#include "../Server.hpp"

void	Server::handleQuit(Client &client, std::vector<std::string> &params) {

	std::string reason = "Client Quit";
	if (!params.empty())
		reason = params[0];

	std::string quitMessage = client.getPrefix() + " QUIT :" + reason + "\r\n";

	std::vector<std::string> clientChannels = getClientChannels(client);
	for (size_t i = 0; i < clientChannels.size(); i++)
	{
		std::map<std::string, Channel*>::iterator it = channels.find(clientChannels[i]);
		if (it == channels.end())
			continue;
		Channel *channel = it->second;
		channel->broadcast(quitMessage, NULL);
		channel->removeMember(client.getNickname());
		if (channel->memberCount() == 0) {
			delete channel;
			channels.erase(it);
		}
		else
			promoteOperatorIfNeeded(channel);
	}
	client.appendToOutputBuffer("ERROR :Closing Link: " + getServerName() + " (" + reason + ")\r\n");
	client.markForRemoval();
}