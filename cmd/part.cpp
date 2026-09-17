#include "../Server.hpp"

void	Server::handlePart(Client &client, std::vector<std::string> &params) {
	if (params.empty()) {
		sendNumericReply(client, ERR_NEEDMOREPARAMS, "PART :Not enough parameters");
		return ;
	}
	if (params[0].empty() || (params[0][0] != '#' && params[0][0] != '&')) { /*manca #*/
		sendNumericReply(client, ERR_NOSUCHCHANNEL, params[0] + " :No such channel");
		return ;
	}

	std::string channelKey = toLowerCase(params[0]);
	channelKey = channelKey.substr(1);
	std::map<std::string, Channel*>::iterator it = channels.find(channelKey);
	if (it == channels.end()) {
		sendNumericReply(client, ERR_NOSUCHCHANNEL, params[0] + " :No such channel");
		return ;
	}

	Channel *channel = it->second;
	if (!channel->isMember(client.getNickname())) {
		sendNumericReply(client, ERR_NOTONCHANNEL, params[0] + " :You're not on that channel");
		return ;
	}

	else {
		std::string partMsg = client.getPrefix() + " PART " + channel->getName();
		if (params.size() > 1 && !params[1].empty())
			partMsg += " :" + params[1];
		partMsg += "\r\n";
		channel->broadcast(partMsg, NULL);
		channel->removeMember(client.getNickname());
		if (channel->memberCount() == 0) {
			delete channel;
			channels.erase(it);
		}
		else
			promoteOperatorIfNeeded(channel);
	}
}
