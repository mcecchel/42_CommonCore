#include "../Server.hpp"

void	Server::handlePrivMsg(Client &client, std::vector<std::string> &params) {
	if (params.empty()) {
		sendNumericReply(client, ERR_NORECIPIENT, ":No recipient given (PRIVMSG)");
		return ;
	}
	if (params.size() < 2 || params[1].empty()) {
		sendNumericReply(client, ERR_NOTEXTTOSEND, ":No text to send");
		return ;
	}
	std::string target = toLowerCase(params[0]);
	if (target[0] == '#' || target[0] == '&') {
		target = target.substr(1);
		std::map<std::string, Channel*>::iterator it = channels.find(target);
		if (it == channels.end()) {
			sendNumericReply(client, ERR_NOSUCHCHANNEL, target + " :No such channel");
			return ;
		}
		Channel *channel = it->second;
		if (!channel->isMember(client.getNickname())) {
			sendNumericReply(client, ERR_CANNOTSENDTOCHAN, target + " :You're not on that channel");
			return ;
		}
		std::string privMsg = client.getPrefix() + " PRIVMSG " + it->second->getName() + " :" + params[1] + "\r\n";
		it->second->broadcast(privMsg, &client);
	}
	else {
		Client *targetClient = NULL;
		std::map<int, Client*>::iterator it;

		for (it = clients.begin(); it != clients.end(); ++it) {
			if (it->second != NULL && ircEqualString(it->second->getNickname(), target)) {
				targetClient = it->second;
				break ;
			}
		}
		if (targetClient == NULL) {
			sendNumericReply(client, ERR_NOSUCHNICK, target + " :No such nick/channel");
			return ;
		}
		std::string privMsg = client.getPrefix() + " PRIVMSG " + targetClient->getDisplayNick() + " :" + params[1] + "\r\n";
		targetClient->appendToOutputBuffer(privMsg);

		client.addPmContact(targetClient->getNickname());
		targetClient->addPmContact(client.getNickname());	
	}
}
