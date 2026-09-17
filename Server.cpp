# include "Server.hpp"

Server::Server(const int port, const std::string pass) {
	if (port < 1024)
		throw BadSetupException("Port must be a value between 1024 and 65535.\n");
	if (pass.empty())
		throw BadSetupException("Password cannot be empty\n");
	this->_port = port;
	this->_psw = pass;
	this->serverName = "ircserv";

	this->setupSocket();
	this->setupSignals();
}

void	Server::setupSocket() {

	this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->server_fd == -1)
		errorSetup("Something went wrong obtaining socket fd\n");

	struct sockaddr_in addr;

	std::memset(&addr, 0, sizeof(addr));   // azzeri tutta la struttura
	addr.sin_family = AF_INET;             // IPv4
	addr.sin_addr.s_addr = INADDR_ANY;     // ascolta su tutte le interfacce
	addr.sin_port = htons(_port);           // porta convertita in network byte order

	int optval = 1;
	if (setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)
		errorSetup("Something went wrong with setting socket option\n");

	if (fcntl(this->server_fd, F_SETFL, O_NONBLOCK) == -1)
		errorSetup("Something went wrong while making non-blocking\n");

	if (bind(this->server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errorSetup("Something went wrong bindind fd\n");

	if(listen(this->server_fd, 100) == -1)
		errorSetup("Something went wrong making fd listen\n");

	setupFdStruct();
}

void Server::setupFdStruct() {
	struct pollfd pollfdStruct;

	pollfdStruct.fd = this->server_fd;
	pollfdStruct.events = POLLIN | POLLHUP;
	pollfdStruct.revents = 0;

	this->fds.push_back(pollfdStruct);
}

void	Server::errorSetup(const std::string msg) {
	close(this->server_fd);
	throw BadSetupException(msg);
}

void	Server::setupSignals() {
	struct sigaction	sa;
	sa.sa_handler = handleSigint;
	sa.sa_flags = 0;
	if(sigemptyset(&sa.sa_mask) == -1)
		errorSetup("Program failed in signal management\n");
	if (sigaction(SIGINT, &sa, NULL) == -1)
		errorSetup("Program closed with keys combination\n");
}

std::string Server::getServerName() {
	return (this->serverName);
}
Server::~Server() {
	serverDestroyer();
}
