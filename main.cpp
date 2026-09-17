#include <cstring>
#include <cstdlib>
#include <iostream>
#include "Server.hpp"

volatile sig_atomic_t g_stop = 0;

void handleSigint(int signum)
{
	(void)signum;
	g_stop = 1;
}

static int	port_check (char *s) {
	char *endptr;
	long res = strtol(s, &endptr, 10);
	if (endptr == s || *endptr != '\0' || res < 0 || res > 65535)
		return (-1);
	return (res);
}

int	main (int argc, char *argv[]) {

	if (argc != 3) {
		std::cout << "Please insert the correct number of parameter" << std::endl;
		std::cout << "./ircserv <port> <password>" << std::endl;
		return (-1);
	}

	int	port = port_check(argv[1]);
	if (port == -1) {
		std::cout << "Please insert valid port" << std::endl;
		return (-1);
	}
	try {
		signal(SIGPIPE, SIG_IGN);

		Server server(port, argv[2]);
		server.run();
	}
	catch (const std::exception& e) {
		std::cerr << "Errore: " << e.what() << std::endl;
		return (1);
	}
}