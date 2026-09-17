# include "Bot.hpp"

# include <fcntl.h>

Bot::Bot(const std::string &host, const std::string &port,
		 const std::string &password)
	: _fd(-1), _password(password), _inputBuffer(), _outputBuffer(),
	  _disabledUsers()
{
	struct addrinfo hints;
	struct addrinfo *result;
	struct addrinfo *current;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	result = NULL;
	if (getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0)
		throw std::runtime_error("unable to resolve server address");

	// Prova gli indirizzi restituiti dal resolver finche' una connessione
	// TCP non riesce (osi' il bot funziona sia con un hostname sia con IP)
	for (current = result; current != NULL; current = current->ai_next) {
		_fd = socket(current->ai_family, current->ai_socktype,
					 current->ai_protocol);
		if (_fd == -1)
			continue ;
		if (connect(_fd, current->ai_addr, current->ai_addrlen) == 0)
			break ;
		close(_fd);
		_fd = -1;
	}
	freeaddrinfo(result);
	if (_fd == -1)
		throw std::runtime_error("unable to connect to IRC server");

	// Anche il bot usa un fd non bloccante, come i client accettati dal
	// server. La connect() resta bloccante: viene completata prima di
	// entrare nel ciclo poll, quindi non serve gestire EINPROGRESS.
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1) {
		close(_fd);
		_fd = -1;
		throw std::runtime_error("unable to set non-blocking socket");
	}

	// Il bot si registra esattamente come un client IRC normale = server
	// richiede PASS, NICK e USER prima di consentire PRIVMSG
	sendLine("PASS " + _password + "\r\n");
	sendLine("NICK bot\r\n");
	sendLine("USER bot 0 * :IRC Bot\r\n");
}

Bot::~Bot()
{
	if (_fd != -1)
		close(_fd);
}

void Bot::sendLine(const std::string &line)
{
	_outputBuffer.append(line);
	flushOutput();
}

void Bot::flushOutput()
{
	while (!_outputBuffer.empty()) {
		ssize_t sent = send(_fd, _outputBuffer.data(), _outputBuffer.size(), 0);
		if (sent > 0) {
			_outputBuffer.erase(0, static_cast<size_t>(sent));
			continue ;
		}
		if (sent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
			return ;
		throw std::runtime_error("send failed");
	}
}

void Bot::sendPrivmsg(const std::string &nickname, const std::string &text)
{
	sendLine("PRIVMSG " + nickname + " :" + text + "\r\n");
}

void Bot::sendTrollArt(const std::string &nickname)
{
	static const char *art[] = {
		"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣤⣤⣤⣤⣤⣴⡶⠶⠶⠶⠶⠶⠶⠶⠶⠤⠤⢤⣤⣤⣤⣤⣤⣄⣀⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
		"⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⠟⠋⠀⠀⠀⠀⢀⣀⠤⠖⠚⢉⣉⣉⣉⣉⣀⠀⠀⠀⠀⠀⠀⠈⠉⠩⠛⠛⠛⠻⠷⣦⣄⡀⠀⠀⠀⠀⠀⠀",
		"⠀⠀⠀⠀⠀⠀⠀⣠⡿⠋⠀⠀⠀⣀⠤⠒⣉⠤⢒⣊⡉⠠⠤⠤⢤⡄⠈⠉⠉⠀⠂⠀⠀⠐⠂⠀⠉⠉⠉⠉⠂⠀⠙⠻⣶⣄⠀⠀⠀⠀",
		"⠀⠀⠀⠀⠀⠀⣰⡿⠁⠀⠀⡠⠊⢀⠔⣫⠔⠊⠁⠀⠀⠀⠀⠀⠀⠙⡄⠀⠀⠀⠀⠀⠘⣩⠋⠀⠀⠀⠉⠳⣄⠀⠀⠀⠈⢻⡇⠀⠀⠀",
		"⠀⠀⠀⠀⠀⣰⡿⠁⠀⠀⠀⠀⠀⠁⠜⠁⣀⣤⣴⣶⣶⣶⣤⣤⣀⠀⠃⠀⠀⠀⠀⠀⠀⠁⠀⠀⠀⠀⠀⠀⠈⠆⠀⠀⠀⠸⣧⡀⠀⠀",
		"⠀⠀⠀⣠⣾⣿⣥⠤⢄⡀⠀⢠⣤⠔⢠⣾⣿⣿⣿⣿⣿⣯⣄⡈⠙⢿⣦⠀⠀⠀⠀⡀⢀⣤⣶⣿⣿⣿⣿⣿⣦⠀⣀⣀⣀⣀⡙⢿⣦⡀",
		"⠀⣠⡾⣻⠋⢀⣠⣴⠶⠾⢶⣤⣄⡚⠉⠉⠉⠁⣠⣼⠏⠉⠙⠛⠷⡾⠛⠀⠀⠀⠘⠛⢿⡟⠛⠋⠉⠉⠉⠁⠀⠀⠀⠀⠀⠦⣝⠦⡙⣿",
		"⢰⡟⠁⡇⢠⣾⠋⠀⠀⣼⣄⠉⠙⠛⠷⠶⠶⠿⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣇⠀⠀⠀⠠⣦⣄⣴⡾⢛⡛⠻⠷⠘⡄⢸⣿",
		"⢸⡇⠀⡇⢸⣇⢀⣤⣴⣿⠻⠷⣦⣄⣀⠀⠀⠀⢀⡀⠀⣀⠰⣤⡶⠶⠆⠀⠀⠀⠀⠀⠈⠛⢿⣦⣄⠀⠈⠉⠉⠁⢸⣇⠀⠀⣠⠃⢸⣿",
		"⠸⣿⡀⢇⠘⣿⡌⠁⠈⣿⣆⠀⠀⠉⢻⣿⣶⣦⣤⣀⡀⠀⠀⢻⣦⠰⡶⠿⠶⠄⠀⠀⠀⣠⣾⠿⠟⠓⠦⡄⠀⢀⣾⣿⡇⢈⠡⠔⣿⡟",
		"⠀⠙⢿⣌⡑⠲⠄⠀⠀⠙⢿⣿⣶⣦⣼⣿⣄⠀⠈⠉⠛⠻⣿⣶⣯⣤⣀⣀⡀⠀⠘⠿⠾⠟⠁⠀⠀⢀⣀⣤⣾⣿⢿⣿⣇⠀⠀⣼⡟⠀",
		"⠀⠀⠀⠹⣿⣇⠀⠀⠀⠀⠈⢻⣦⠈⠙⣿⣿⣷⣶⣤⣄⣠⣿⠁⠀⠈⠉⠙⢻⡟⠛⠻⠿⣿⠿⠛⠛⢻⣿⠁⢈⣿⣨⣿⣿⠀⢰⡿⠀⠀",
		"⠀⠀⠀⠀⠈⢻⣇⠀⠀⠀⠀⠀⠙⢷⣶⡿⠀⠈⠙⠛⠿⣿⣿⣶⣶⣦⣤⣤⣼⣧⣤⣤⣤⣿⣦⣤⣤⣶⣿⣷⣾⣿⣿⣿⡟⠀⢸⡇⠀⠀",
		"⠀⠀⠀⠀⠀⠈⢿⣦⠀⠀⠀⠀⠀⠀⠙⢷⣦⡀⠀⠀⢀⣿⠁⠉⠙⠛⠻⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⢸⣷⠀⠀",
		"⠀⠀⠀⠀⠀⠀⠀⠙⢷⣄⠀⢀⡀⠀⣀⡀⠈⠻⢷⣦⣾⡃⠀⠀⠀⠀⠀⢸⡇⠀⠀⠀⢹⡟⠉⠉⣿⠏⢡⣿⠃⣾⣷⡿⠁⠀⠘⣿⠀⠀",
		"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢷⣤⣉⠒⠤⣉⠓⠦⣀⡈⠉⠛⠿⠶⢶⣤⣤⣾⣧⣀⣀⣀⣿⣄⣠⣼⣿⣤⣿⠷⠾⠟⠋⠀⠀⠀⠀⣿⠀⠀",
		"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠿⣶⣄⡉⠒⠤⢌⣑⠲⠤⣀⡀⠀⠀⠀⠈⠍⠉⠉⠉⠉⠉⠁⠀⠀⠀⠀⠀⣠⠏⠀⢰⠀⠀⣿⡄⠀",
		"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠛⠿⢷⣦⣄⡉⠑⠒⠪⠭⢄⣀⣀⠀⠐⠒⠒⠒⠒⠀⠀⠐⠒⠊⠉⠀⢀⡠⠚⠀⠀⢸⡇⠀",
		"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠻⢷⣦⣀⠀⠀⠀⠀⠀⠀⠉⠉⠉⠉⠉⠉⠓⠒⠒⠒⠊⠁⠀⠀⠀⢠⣿⠃⠀",
		"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠛⠛⠷⠶⣶⣦⣄⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣠⣴⠟⠁⠀⠀",
		"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠙⠛⠛⠷⠶⠶⠶⠶⠶⠾⠛⠛⠉⠀⠀⠀⠀⠀"
	};
	size_t lineCount = sizeof(art) / sizeof(art[0]);
	// Ogni riga e' un PRIVMSG autonomo: i newline dentro un PRIVMSG
	// verrebbero interpretati dal server come nuovi comandi IRC.
	for (size_t i = 0; i < lineCount; ++i)
		sendPrivmsg(nickname, art[i]);
}

bool Bot::equalsIgnoreCase(const std::string &left, const std::string &right) const
{
	return (lowerCase(left) == lowerCase(right));
}

std::string Bot::lowerCase(const std::string &value) const
{
	std::string result = value;
	for (size_t i = 0; i < result.size(); ++i)
		if (result[i] >= 'A' && result[i] <= 'Z')
			result[i] = static_cast<char>(result[i] - 'A' + 'a');
	return (result);
}

std::string Bot::currentTime() const
{
	char buffer[64];
	std::time_t now = std::time(NULL);
	std::tm *local = std::localtime(&now);

	if (local == NULL || std::strftime(buffer, sizeof(buffer),
									   "%Y-%m-%d %H:%M:%S", local) == 0)
		return ("time unavailable");
	return (std::string(buffer));
}

void Bot::handlePrivmsg(const std::string &sender, const std::string &text)
{
	std::string command = lowerCase(text);
	bool disabled = (_disabledUsers.find(lowerCase(sender))
					 != _disabledUsers.end());

	// !bot on e' sempre processato, altrimenti un utente disattivato non
	// potrebbe piu' riattivare le risposte del bot
	if (command == "!bot on") {
		_disabledUsers.erase(lowerCase(sender));
		sendPrivmsg(sender, "bot attivato per " + sender);
		return ;
	}
	if (command == "!bot off") {
		_disabledUsers.insert(lowerCase(sender));
		sendPrivmsg(sender, "bot disattivato per " + sender);
		return ;
	}
	if (disabled)
		return ;

	if (command == "!help")
		sendPrivmsg(sender, "comandi: !help, !troll, !game, !time, !about, !bot on, !bot off");
	else if (command == "!troll")
		sendTrollArt(sender);
	else if (command == "!game")
		sendPrivmsg(sender, "You just lost the game :(");
	else if (command == "!time")
		sendPrivmsg(sender, currentTime());
	else if (command == "!about")
		sendPrivmsg(sender, "IRC bot standalone, nickname bot");
}

void Bot::handleLine(const std::string &line)
{
	if (line.compare(0, 5, "PING ") == 0) {
		sendLine("PONG " + line.substr(5) + "\r\n");
		return ;
	}

	// Per un PRIVMSG il prefisso contiene il mittente e la parte dopo ': ' contiene il testo
	// Le altre risposte numeriche vengono semplicemente ignorate: servono solo a completare la registrazione del client bot.
	size_t commandPos = line.find(" PRIVMSG ");
	if (commandPos == std::string::npos || line.empty() || line[0] != ':')
		return ;
	size_t bang = line.find('!', 1);
	size_t targetStart = commandPos + 9;
	size_t targetEnd = line.find(' ', targetStart);
	size_t textStart = line.find(" :", targetEnd);
	if (bang == std::string::npos || targetEnd == std::string::npos
		|| textStart == std::string::npos)
		return ;
	if (!equalsIgnoreCase(line.substr(targetStart, targetEnd - targetStart), "bot"))
		return ;
	handlePrivmsg(line.substr(1, bang - 1), line.substr(textStart + 2));
}

void Bot::run()
{
	struct pollfd socketPoll;
	char buffer[1024];

	socketPoll.fd = _fd;
	while (true)
	{
		socketPoll.events = POLLIN;
		if (!_outputBuffer.empty())
			socketPoll.events |= POLLOUT;
		socketPoll.revents = 0;
		int pollResult = poll(&socketPoll, 1, -1);
		if (pollResult < 0)
		{
			if (errno == EINTR)
				continue ;
			throw std::runtime_error("poll failed");
		}
		if (socketPoll.revents & (POLLERR | POLLHUP | POLLNVAL))
			throw std::runtime_error("IRC server disconnected");
		if (socketPoll.revents & POLLOUT)
			flushOutput();
		if (!(socketPoll.revents & POLLIN))
			continue ;

		ssize_t received = recv(_fd, buffer, sizeof(buffer), 0);
		if (received == 0)
			throw std::runtime_error("IRC server disconnected");
		if (received == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				continue ;
			throw std::runtime_error("receive failed");
		}
		_inputBuffer.append(buffer, static_cast<size_t>(received));
		size_t lineEnd;
		while ((lineEnd = _inputBuffer.find('\n')) != std::string::npos)
		{
			std::string line = _inputBuffer.substr(0, lineEnd);
			_inputBuffer.erase(0, lineEnd + 1);
			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);
			handleLine(line);
		}
	}
}