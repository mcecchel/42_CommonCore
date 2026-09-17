#include "Server.hpp"
#include <iomanip>

bool	Server::ircEqualString(const std::string &a, const std::string &b) {
	if (a.size() != b.size())
		return (false);

	for (std::size_t i = 0; i < a.size(); i++) {
		unsigned char ca = static_cast<unsigned char>(a[i]);
		unsigned char cb = static_cast<unsigned char>(b[i]);
		if (std::tolower(ca) != std::tolower(cb))
			return (false);
	}
	return (true);
}

struct pollfd	Server::makePollfd(int _fd, short _events) {
	struct pollfd	pollfdStruct;
	pollfdStruct.fd = _fd;
	pollfdStruct.events = _events;
	pollfdStruct.revents = 0;

	return (pollfdStruct);
}

std::vector<std::string> Server::getClientChannels(const Client &client) const
{
	std::vector<std::string> result;
	std::map<std::string, Channel*>::const_iterator it;
	
	for (it = channels.begin(); it != channels.end(); ++it) {
		if (it->second->isMember(client.getNickname()))
			result.push_back(it->first);
	}
	return (result);
}

void	Server::sendWelcomeSequence(Client& client) {
	sendNumericReply(client, RPL_WELCOME, ":Welcome to the Internet Relay Network " + client.getDisplayNick() + "!" + client.getUsername() + "@host");
	sendNumericReply(client, RPL_YOURHOST,":Your host is " + getServerName() + ", running version 1.0");
	sendNumericReply(client, RPL_CREATED, ":This server was created today");
	sendNumericReply(client, RPL_MYINFO, getServerName() + " 1.0  itkol");
}


void	Server::sendNumericReply(Client& client, NumericReply code, const std::string& args)
{
	std::string target = client.getDisplayNick();
	if (target.empty())
		target = "*";

	std::ostringstream ss;
	ss << std::setfill('0') << std::setw(3) << static_cast<int>(code);

	std::string reply = ":" + getServerName() + " ";
	reply += ss.str() + " ";
	reply += target + " ";
	reply += args + "\r\n";
	client.appendToOutputBuffer(reply);
}

void	Server::handlePing(Client &client, std::vector<std::string> &params)
{
	std::string token = params.empty() ? getServerName() : params[0];

	std::string pong = ":" + getServerName() + " PONG " + getServerName()
		+ " :" + token + "\r\n";
	client.appendToOutputBuffer(pong);
}

void Server::promoteOperatorIfNeeded(Channel *channel)
{
	if (channel->memberCount() == 0 || channel->hasOperators())
		return ;

	std::map<std::string, Channel::ChannelMember>::const_iterator it = channel->getMembers().begin();
	channel->setOperator(it->first, true);

	std::string modeMsg = ":" + getServerName() + " MODE " + channel->getName()
		+ " +o " + it->second.client->getDisplayNick() + "\r\n";
	channel->broadcast(modeMsg, NULL);
}

Server::BadSetupException::BadSetupException(const std::string& msg)
	: _msg(msg) {}

Server::BadSetupException::~BadSetupException() throw() {}

const char* Server::BadSetupException::what() const throw() {
	return _msg.c_str();
}