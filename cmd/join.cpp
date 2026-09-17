#include "../Server.hpp"
#include "../Channel.hpp"

std::string	Server::toLowerCase(const std::string &str)
{
	std::string result = str;

	for (size_t i = 0; i < result.size(); i++)
		result[i] = std::tolower(static_cast<unsigned char>(result[i]));
	return (result);
}

bool	Server::isChanNameTaken(const std::string& str) {
	std::map<std::string, Channel*>::const_iterator it = channels.find(str);
	if (it == this->channels.end() || it->second == NULL)
		return (false);
	return (true);
}

void	Server::handleJoin(Client &client, std::vector<std::string> &params) {
	if (params.empty() || params[0].empty()) { /*manca parametro nome canale*/
		sendNumericReply(client, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
		return ;
	}
	if(params[0][0] != '#' && params[0][0] != '&') { /*manca #*/
		sendNumericReply(client, ERR_NOSUCHCHANNEL, params[0] + " :No such channel");
		return ;
	}
	std::string	channelKey = toLowerCase(params[0]);
	channelKey = channelKey.substr(1);
	std::map<std::string, Channel*>::const_iterator it = channels.find(channelKey);
	if (it == channels.end()) {
		Channel *newChannel = new Channel(params[0]);
		channels[channelKey] = newChannel;
		newChannel->addMember(&client, true);
		sendJoinSuccess(client, newChannel);
	}
	else {
		Channel *channel = it->second;
		if(channel->hasKey()) {
			if (params.size() < 2 || params[1] != channel->getKey()) {
				sendNumericReply(client, ERR_BADCHANNELKEY, channelKey + " :Cannot join channel (+k)");
				return ;
			}
		}
		if (channel->isMember(client.getNickname()))
  			  return; 
		if (channel->isInviteOnly() && !channel->isInvited(client.getNickname())) {
			sendNumericReply(client, ERR_INVITEONLYCHAN, channelKey + " :Cannot join channel (+i)");
			return ;
		}
		if(channel->hasUserLimit()) {
			if (channel->memberCount() >= static_cast<size_t>(channel->getUserLimit())) {
				sendNumericReply(client, ERR_CHANNELISFULL, channelKey + " :Cannot join channel (+l)");
				return ;
			}
		}
		channel->addMember(&client);
		if (channel->isInvited(client.getNickname()))
			channel->removeInvite(client.getNickname());
		sendJoinSuccess(client, channel);
	}
}

void Server::sendJoinSuccess(Client &client, Channel *channel)
{
	std::string joinMsg = client.getPrefix() + " JOIN :" + channel->getName() + "\r\n";
	channel->broadcast(joinMsg, NULL);   // NULL = nessuno escluso, anche il nuovo membro lo riceve

	if (!channel->getTopic().empty())
		sendNumericReply(client, RPL_TOPIC, channel->getName() + " :" + channel->getTopic());
	else
		sendNumericReply(client, ERR_NOTOPIC, channel->getName() + " :No topic is set");

	std::string names;
	std::map<std::string, Channel::ChannelMember>::const_iterator it;

	for (it = channel->getMembers().begin(); it != channel->getMembers().end(); ++it) {
		if (it->second.isOperator)
			names += "@";
		names += it->second.client->getDisplayNick() + " ";
	}
	sendNumericReply(client, RPL_NAMREPLY, "= " + channel->getName() + " :" + names);
	sendNumericReply(client, RPL_ENDOFNAMES, channel->getName() + " :End of NAMES list");
}

