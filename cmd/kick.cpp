// Sintassi:   KICK <channel> <nickname>{,<nickname>} [:<motivo>]
// Esempio:               KICK #general Mario
// Esempio con motivo:    KICK #general Mario :Comportamento scorretto
// Note: il formato standard permette di espellere più nickname in una sola
// chiamata (separati da virgola); per ft_irc è comune e legittimo limitarsi
// a un solo bersaglio per chiamata.

// Ordine dei controlli:

//     1. params.size() < 2
//         -> ERR_NEEDMOREPARAMS, args = "KICK :Not enough parameters"
//     2. cerca il canale (params[0]) in "channels"
//         -> se non esiste: ERR_NOSUCHCHANNEL, args = params[0] + " :No such channel"
//     3. controlla che il MITTENTE sia membro del canale
//         -> se no: ERR_NOTONCHANNEL, args = params[0] + " :You're not on that channel"
//     4. controlla che il mittente sia OPERATORE del canale
//         -> se no: ERR_CHANOPRIVSNEEDED, args = params[0] + " :You're not channel operator"
//     5. cerca il client bersaglio (params[1]) tra i membri del canale
//         -> se non è membro: ERR_USERNOTINCHANNEL,
//            args = params[1] + " " + params[0] + " :They aren't on that channel"
//     6. successo:
//         motivo = params[2] se presente, altrimenti il nickname del mittente
//         kickMsg = ":" + nickMittente + "!user@host KICK " + params[0] + " "
//                   + params[1] + " :" + motivo + "\r\n"
//         broadcast di kickMsg a TUTTI i membri del canale, incluso il kickato
//         rimuovi il bersaglio dalla lista membri del Channel
//         rimuovi il canale dalla lista "appartenenze" del suo Client
//         (se il bersaglio era anche operatore, rimuovilo anche dalla lista operatori)
#include "../Server.hpp"

void Server::handleKick(Client &client, std::vector<std::string> &params)
{
	// controllo numero parametri
	if (params.size() < 2)
	{
		sendNumericReply(client, ERR_NEEDMOREPARAMS, "KICK :Not enough parameters");
		return ;
	}
	// validazione prefisso del canale
	if (params[0].empty() || (params[0][0] != '#' && params[0][0] != '&'))
	{
		sendNumericReply(client, ERR_NOSUCHCHANNEL, params[0] + " :No such channel");
		return ;
	}
	// normalizzazione nome
	std::string channelKey = toLowerCase(params[0]);
	channelKey = channelKey.substr(1);
	// ricerca del canale in channels
	std::map<std::string, Channel*>::iterator it = channels.find(channelKey);
	if (it == channels.end())
	{
		sendNumericReply(client, ERR_NOSUCHCHANNEL, params[0] + " :No such channel");
		return ;
	}

	// recupera l'oggetto Channel dopo aver trovato la chiave nella mappa
	Channel *channel = it->second;
	// il mittente deve appartenere al canale per poter eseguire KICK
	if (!channel->isMember(client.getNickname()))
	{
		sendNumericReply(client, ERR_NOTONCHANNEL,
			params[0] + " :You're not on that channel");
		return ;
	}
	// solo un operatore del canale puo' espellere un altro membro
	if (!channel->isOperator(client.getNickname()))
	{
		sendNumericReply(client, ERR_CHANOPRIVSNEEDED,
			params[0] + " :You're not channel operator");
		return ;
	}

	// cerca il nick da togliere tra i membri del canale
	std::string targetNick = toLowerCase(params[1]);
	std::map<std::string, Channel::ChannelMember>::const_iterator targetIt;
	targetIt = channel->getMembers().find(targetNick);
	if (targetIt == channel->getMembers().end())
	{
		sendNumericReply(client, ERR_USERNOTINCHANNEL,
			params[1] + " " + params[0] + " :They aren't on that channel");
		return ;
	}
	// parser mette il testo dopo ':' in params[2]
	std::string reason = client.getDisplayNick();
	if (params.size() > 2 && !params[2].empty())
		reason = params[2];
	// comunica il KICK a tutti prima di rimuovere il bersaglio dal canale
	std::string kickMsg = client.getPrefix() + " KICK " + channel->getName() + " "
		+ targetIt->second.client->getDisplayNick() + " :" + reason + "\r\n";
	channel->broadcast(kickMsg, NULL);
	channel->removeMember(targetNick);
	if (channel->memberCount() == 0) {
		delete channel;
		channels.erase(it);
	}
	else
		promoteOperatorIfNeeded(channel);
}