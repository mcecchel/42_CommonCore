#include "../Server.hpp"

void	Server::handleTopic(Client &client, std::vector<std::string> &params) {
	if (params.empty())
	{
		sendNumericReply(client, ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
		return ;
	}
	if (params[0].empty() || (params[0][0] != '#' && params[0][0] != '&')) {
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
	// No 2^ parametro + il client vuole VEDERE il topic attuale
	if (params.size() < 2) {
		if (channel->getTopic().empty())
			sendNumericReply(client, ERR_NOTOPIC, channel->getName() + " :No topic is set");
		else {
			sendNumericReply(client, RPL_TOPIC, channel->getName() + " :" + channel->getTopic());
			std::ostringstream topicInfo;
			topicInfo << channel->getName() << " " << channel->getTopicSetBy()
				<< " " << channel->getTopicSetTime();
			sendNumericReply(client, RPL_TOPICWHOTIME, topicInfo.str());
		}
		return ;
	}
	// 2^ parametro presente + client vuole IMPOSTARE/cancellare il topic
	if (channel->isTopicRestricted() && !channel->isOperator(client.getNickname()))
	{
		sendNumericReply(client, ERR_CHANOPRIVSNEEDED, channel->getName() + " :You're not channel operator");
		return ;
	}
	channel->setTopic(params[1]);
	channel->setTopicMetadata(client.getDisplayNick(), time(NULL));
	std::string topicMessage = client.getPrefix() + " TOPIC " + channel->getName() + " :" + channel->getTopic() + "\r\n";
	channel->broadcast(topicMessage, NULL);
	// TODO: RPL_TOPICWHOTIME (333, "chi ha messo il topic e quando")
	// Da aggiungere due nuovi campi a Channel (_topicSetBy, _topicSetTime) con
	// relativi getter/setter (modifica a Channel.hpp)
}