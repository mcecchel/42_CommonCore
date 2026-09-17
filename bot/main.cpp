# include "Bot.hpp"

# include <cstdlib>
# include <exception>
# include <iostream>
# include <signal.h>

int main(int argc, char **argv)
{
	if (argc != 4) {
		std::cerr << "Usage: " << argv[0] << " <host> <port> <password>" << std::endl;
		return (1);
	}
	try {
		// Evita la terminazione automatica se il server chiude il socket
		// mentre il bot sta inviando una risposta.
		signal(SIGPIPE, SIG_IGN);
		Bot bot(argv[1], argv[2], argv[3]);
		bot.run();
	}
	catch (const std::exception &error) {
		std::cerr << "bot: " << error.what() << std::endl;
		return (1);
	}
	return (0);
}
