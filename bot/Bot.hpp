#ifndef BOT_HPP
# define BOT_HPP

# include <netdb.h>
# include <set>
# include <string>
# include <cerrno>
# include <cstdio>
# include <cstring>
# include <ctime>
# include <iostream>
# include <poll.h>
# include <stdexcept>
# include <sys/socket.h>
# include <unistd.h>

class Bot {
	private:
		int						_fd;
		std::string				_password;
		std::string				_inputBuffer;
		std::string				_outputBuffer;
		std::set<std::string>	_disabledUsers;

		Bot(const Bot &other);
		Bot &operator=(const Bot &other);

		void		sendLine(const std::string &line);
		void		flushOutput();
		void		sendPrivmsg(const std::string &nickname, const std::string &text);
		void		sendTrollArt(const std::string &nickname);
		void		handleLine(const std::string &line);
		void		handlePrivmsg(const std::string &sender, const std::string &text);
		bool		equalsIgnoreCase(const std::string &left, const std::string &right) const;
		std::string	lowerCase(const std::string &value) const;
		std::string	currentTime() const;

	public:
		Bot(const std::string &host, const std::string &port, const std::string &password);
		~Bot();

		void	run();
};

#endif