#include "Server.hpp"

void	Server::run() {
	while (!g_stop)
	{
		// 1. aggiornare gli eventi POLLOUT per ogni client (updatePollEvents)
		updatePollEvents();
		if (poll(&fds[0], fds.size(), -1) == -1) {
			if (errno == EINTR) //continua per ctrl +C
				continue;
			else {
				std::cerr << "poll() failed: " << strerror(errno) <<std::endl;//break; with delete managing
				break;
			}
		}
		else
			handlePollEvents();
		cleanupClients();// pulizia finale: chiudere tutti i socket rimasti aperti
	}
}

void	Server::updatePollEvents() {
	for (size_t idx = 1; idx < this->fds.size(); idx++) {
		int	fd = this->fds[idx].fd;

		std::map<int, Client*>::iterator it = this->clients.find(fd);
		if (it == this->clients.end() || it->second == NULL) {
			this->fds[idx].events &= ~POLLOUT;
			continue ;
		}
		Client* client = it->second;
		// Anche un client in fase di rimozione deve poter svuotare
		// l'output buffer (es. l'ERROR di QUIT) prima di essere chiuso.
		if (client->hasDataToSend())
			this->fds[idx].events |= POLLOUT;
		else
			this->fds[idx].events &= ~POLLOUT;
	}
}

void	Server::handlePollEvents() {
	for (size_t idx = 0; idx < fds.size(); idx++) {
		//std::cout << "[DEBUG] fd=" << fds[idx].fd << " revents=" << fds[idx].revents << std::endl;
		if (fds[idx].revents & POLLIN) {
			if (idx == 0)
				acceptNewClient();
			else
				handleClientRead(fds[idx].fd);
		}
		if(fds[idx].revents & POLLOUT) {
			handleClientWrite(fds[idx].fd);
		}
		if (fds[idx].revents & (POLLHUP | POLLERR)) {
			std::vector<std::string> n;
			handleQuit(*(clients[fds[idx].fd]), n);
		}
	}
}

void	Server::acceptNewClient() {
	int	newFd = accept(server_fd, NULL, NULL);
	if (newFd < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) //nessuna operazione pronta ora
			return ;
		else {
			std::cerr << "Fail to accept new client" << std::endl;
			return ;
		}
	}
	else {
		struct pollfd newClientFd = makePollfd(newFd, POLLIN);
		if (fcntl(newFd, F_SETFL, O_NONBLOCK) == -1) {
			std::cerr << "Something went wrong while set non-blocking client\n";
			close(newFd);
			return ;
		}
		Client* newClient = NULL;
		try {									//rollback per chiudere in modo pulito
			newClient = new Client(newFd);
			clients.insert(std::make_pair(newFd, newClient));
			this->fds.push_back(newClientFd);
		}
		catch (const std::bad_alloc& e) {
			std::cerr << "Allocation failed in new client creation\n";
			if (newClient) {
				clients.erase(newFd);
				delete (newClient);
			}
			close (newFd);
			return ;
		}
	}
}

ssize_t Server::handleRecv(int fd)
{
	std::map<int, Client*>::iterator it = this->clients.find(fd);
	if (it == this->clients.end() || it->second == NULL)
		return (0);// Client non trovato o ptr nullo
	
	char buffer[1024];// temporaneo, per lettura dati
	ssize_t bytesRead = recv(fd, buffer, sizeof(buffer), 0);
	if (bytesRead > 0)
	{
		// Aggiungo dati letti al buffer del client
		it->second->appendToInputBuffer(std::string(buffer, bytesRead));
		return (bytesRead);
	}
	else if (bytesRead == 0)
	{
		// Client ha chiuso la connessione
		it->second->markForRemoval();
		return (0);
	}
	else
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return (0); // No dati disponibili al momento ma non e' un errore
		// Errore nella ricezione dei dati
		it->second->markForRemoval();
		return (0);
	}
}

void	Server::handleClientRead(int _fd) {
	std::map<int, Client*>::iterator it = this->clients.find(_fd);
	if (it == this->clients.end() || it->second == NULL)
		return ;// Client non trovato o ptr nullo

	Client* client = it->second;
	if (client->hasToBeRemoved())
		return ;

	if (this->handleRecv(_fd) <= 0) {
		client->markForRemoval();
		return ;
	}

	size_t	pos;
	while (!client->hasToBeRemoved() &&((pos = client->readInputBuffer().find("\n")) != std::string::npos))
	{
		std::string cmd = client->readInputBuffer().substr(0, pos);
		if (!cmd.empty() && cmd[cmd.size() - 1] == '\r')
			cmd.erase(cmd.size() - 1);// Se presente rimuove il carattere '\r'
		client->eraseFromInputBuffer(pos + 1);
		// Parsing della riga in comando + parametri
		this->handleParsing(*client, cmd);
	}
}

void	Server::handleClientWrite(int _fd) {
	std::map<int, Client*>::iterator it = this->clients.find(_fd);
	if (it == this->clients.end() || it->second == NULL)
		//ERRORMESSAGE
		return ;// Client non trovato o ptr nullo
	Client* client = it->second;
	if (!client->hasDataToSend())
		return ;
	std::string buffer = client->readOutputBuffer();
	ssize_t byteSent = send(_fd, buffer.data(), buffer.size(), 0);
	if (byteSent > 0)
		client->eraseFromOutputBuffer(byteSent);
	else if (byteSent == -1) {
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return ;
		else {
			std::cerr << "Send error on fd " << _fd << ": " << strerror(errno) << std::endl;
			client->eraseFromOutputBuffer(client->readOutputBuffer().size());
			client->markForRemoval();
		}
	}
}