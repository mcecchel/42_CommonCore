#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <poll.h>
# include <iostream>
# include <sstream>
# include <string>
# include <ctime> 
# include <vector>
# include <limits>
# include <exception>
# include <signal.h>
# include <errno.h>
# include <cstring>
# include <unistd.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
#include <cctype>
#include <cstdlib>

# include "Client.hpp"
# include "Channel.hpp"
# include "NumericReplies.hpp"

extern volatile sig_atomic_t g_stop;
void handleSigint(int signum);

class Client;
class Channel;
class Server {
	public:
		Server(const int port, const std::string psw);
		void	run();
		~Server();


	private:
		//negare la possibilità di copia
		Server(const Server& other);
		Server& operator=(const Server& other);

		int					_port;		//porta del server 
		std::string			_psw;		//password del server
		std::string			serverName;
		int					server_fd;
		
		std::vector<struct pollfd>		fds;
		std::vector<int>				toRemove;
		std::map<int, Client*>			clients;		// fd → client
		std::map<std::string, Channel*>	channels;		// nome → canale
		

		void			errorSetup(const std::string msg);
		void			removeClient(int fd);
		void			cleanupClients();
		void			serverDestroyer();

		void			setupSocket();
		void			setupSignals();
		void 			setupFdStruct();

		struct pollfd	makePollfd(int _fd, short events);

		void			updatePollEvents();
		void			handlePollEvents();

		void			handleClientRead(int _fd);
		void			handleClientWrite(int _fd);

		void			acceptNewClient();
		ssize_t			handleRecv(int fd);
		void			sendWelcomeSequence(Client& client);

		bool			isCommandAllowed(const Client &client, const std::string &command) const;
		bool 			isChannelOperator(const Client &client, const Channel &channel) const;

		void			validatePassword(Client &client, std::vector<std::string> &params);

		void			handleNick(Client &client, std::vector<std::string> &params);
		bool			isValidNickname(const std::string &nick);
		bool			isNicknameTaken(const std::string& str, const Client *self = NULL);

		void			handleUser(Client &client, std::vector<std::string> &params);
		void			handleParsing(Client &client, const std::string &line);
		
		
		void			handlePrivMsg(Client &client, std::vector<std::string> &params);
		void			handleQuit(Client &client, std::vector<std::string> &params);
		
		void			handleJoin(Client &client, std::vector<std::string> &params);
		void			sendJoinSuccess(Client &client, Channel *channel);
		bool			isChanNameTaken(const std::string& str);
		void			handleKick(Client &client, std::vector<std::string> &params);
		void			handleInvite(Client &client, std::vector<std::string> &params);
		void			handleTopic(Client &client, std::vector<std::string> &params);
		void			handlePart(Client &client, std::vector<std::string> &params);
		void			handlePing(Client &client, std::vector<std::string> &params); //server utils

		void			sendChannelModeIs(Client &client, Channel *channel);
		
		struct ModeChange
		{
			char		sign;
			char		letter;
			std::string	param;
		};

		bool			parseModeChanges(Client &client, Channel *channel,
							const std::string &channelName, std::vector<std::string> &params,
							std::vector<ModeChange> &changes);
		void			applyModeChanges(Client &client, Channel *channel,
							const std::string &channelName, std::vector<ModeChange> &changes);
		void			handleMode(Client &client, std::vector<std::string> &params);

		std::string		toLowerCase(const std::string &str);
		std::string		getServerName();

		bool			ircEqualString(const std::string &a, const std::string &b);
		void			sendNumericReply(Client& client, NumericReply code, const std::string& args);
		void			promoteOperatorIfNeeded(Channel *channel);

		std::vector<std::string>	getClientChannels(const Client &client) const;
		class BadSetupException : public std::exception {
			public:
				BadSetupException(const std::string& msg);
				virtual ~BadSetupException() throw();
				const char* what() const throw();
			private:
				std::string _msg;
		};
};

#endif