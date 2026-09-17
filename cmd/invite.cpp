
#include "../Server.hpp"

void Server::handleInvite(Client &client, std::vector<std::string> &params)
{
	if (params.size() < 2)
	{
		sendNumericReply(client, ERR_NEEDMOREPARAMS, "INVITE :Not enough parameters");
		return ;
	}
	// 2^ param deve essere un nome di canale valido
	if (params[1].empty() || (params[1][0] != '#' && params[1][0] != '&'))
	{
		sendNumericReply(client, ERR_NOSUCHCHANNEL, params[1] + " :No such channel");
		return ;
	}
	// uso la stessa chiave interna adottata da JOIN
	std::string channelKey = toLowerCase(params[1]);
	channelKey = channelKey.substr(1);
	std::map<std::string, Channel*>::iterator it = channels.find(channelKey);
	if (it == channels.end())
	{
		sendNumericReply(client, ERR_NOSUCHCHANNEL, params[1] + " :No such channel");
		return ;
	}
	Channel *channel = it->second;
	// il mittente deve appartenere al canale per poter invitare qualcuno
	if (!channel->isMember(client.getNickname()))
	{
		sendNumericReply(client, ERR_NOTONCHANNEL_INVITE,
			params[1] + " :You're not on that channel");
		return ;
	}
	// Nei canali invite-only solo un operatore puo' usare INVITE.
	if (channel->isInviteOnly()
		&& !channel->isOperator(client.getNickname()))
	{
		sendNumericReply(client, ERR_CHANOPRIVSNEEDED,
			params[1] + " :You're not channel operator");
		return ;
	}
	// Cerca il destinatario tra i client con confronto case-insensitive
	Client *target = NULL;
	std::map<int, Client*>::iterator clientIt;
	for (clientIt = clients.begin(); clientIt != clients.end(); ++clientIt)
	{
		if (clientIt->second != NULL
			&& ircEqualString(clientIt->second->getNickname(), params[0]))
		{
			target = clientIt->second;
			break ;
		}
	}
	if (target == NULL)
	{
		sendNumericReply(client, ERR_NOSUCHNICK, params[0] + " :No such nick/channel");
		return ;
	}
	// Un utente gia' presente nel canale non puo' essere invitato
	std::cout << "[DEBUG] target nick: '" << target->getNickname() 
		<< "' isMember: " << channel->isMember(target->getNickname()) << std::endl;
	if (channel->isMember(target->getNickname()))
	{
		sendNumericReply(client, ERR_USERONCHANNEL,
			target->getDisplayNick() + " " + channel->getName() + " :is already on channel");
		return ;
	}
	// Registra l'invito, conferma l'operazione al mittente e avvisa il destinatario
	channel->addInvite(target->getNickname());
	sendNumericReply(client, RPL_INVITING,
		params[1] + " " + target->getDisplayNick());
	target->appendToOutputBuffer(client.getPrefix() + " INVITE "
		+ target->getDisplayNick() + " " + params[1] + "\r\n");
}
