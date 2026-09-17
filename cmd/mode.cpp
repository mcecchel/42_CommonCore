#include "../Server.hpp"

void	Server::handleMode(Client &client, std::vector<std::string> &params)
{
	if (params.empty()) {
		sendNumericReply(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
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

	// Nessun modestring dopo il nome canale = query, non modifica.
	// Chiunque nel canale può chiedere lo stato attuale, non serve essere operatore.
	if (params.size() < 2) {
		sendChannelModeIs(client, channel);
		return ;
	}

	// Da qui in poi si tratta di una modifica reale: serve essere operatore.
	if (!channel->isOperator(client.getNickname())) {
		sendNumericReply(client, ERR_CHANOPRIVSNEEDED, params[0] + " :You're not channel operator");
		return ;
	}

	// Fase 1: parsing/validazione (nessun side-effect su Channel).
	// Se qualcosa non va, si esce senza aver toccato lo stato del canale.
	std::vector<ModeChange> changes;
	if (!parseModeChanges(client, channel, params[0], params, changes))
		return ;

	// Fase 2: applicazione + broadcast di un'unica riga MODE cumulativa.
	applyModeChanges(client, channel, channel->getName(), changes);
}

bool	Server::parseModeChanges(Client &client, Channel *channel,
	const std::string &channelName, std::vector<std::string> &params,
	std::vector<ModeChange> &changes)
{
	const std::string &modes = params[1];   // es. "+it", "-k", "+o-l"

	if (modes.empty() || (modes[0] != '+' && modes[0] != '-')) {
		sendNumericReply(client, ERR_UNKNOWNMODE, modes + " :is unknown mode char to me");
		return (false);
	}

	char	sign = '+';
	// nextParamIdx punta al prossimo parametro "posizionale" da consumare
	size_t	nextParamIdx = 2;

	for (size_t i = 0; i < modes.size(); i++) {
		char c = modes[i];
		if (c == '+' || c == '-') {
			sign = c;   // cambia il segno per le lettere seguenti
			continue ;
		}

		ModeChange change;
		change.sign = sign;
		change.letter = c;

		if (c == 'i' || c == 't')
			changes.push_back(change);   // nessun parametro, valgono per entrambi i segni
		else if (c == 'k') {
			// la chiave si specifica solo quando la si imposta (+k);
			// per rimuoverla (-k) non serve alcun parametro
			if (sign == '+') {
				if (nextParamIdx >= params.size()) {
					sendNumericReply(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
					return (false);
				}
				change.param = params[nextParamIdx];
				nextParamIdx++;
			}
			changes.push_back(change);
		}
		else if (c == 'o')
		{
			// +o e -o richiedono sempre un nick
			if (nextParamIdx >= params.size()) {
				sendNumericReply(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
				return (false);
			}
			std::string targetNick = toLowerCase(params[nextParamIdx]);
			nextParamIdx++;
			if (!channel->isMember(targetNick)) {
				sendNumericReply(client, ERR_USERNOTINCHANNEL,
					params[nextParamIdx - 1] + " " + channelName + " :They aren't on that channel");
				return (false);
			}
			change.param = targetNick;
			changes.push_back(change);
		}
		else if (c == 'l') {
			// il limite si specifica solo per +l; -l non richiede parametro
			if (sign == '+') {
				if (nextParamIdx >= params.size()) {
					sendNumericReply(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
					return (false);
				}

				const std::string& limitStr = params[nextParamIdx];
				if (limitStr.empty()) {
					sendNumericReply(client, ERR_INVALIDMODEPARAM,
						channelName + " l " + limitStr + " :Invalid limit value");
					return (false);
				}
				// deve essere un numero: nessun carattere non-cifra ammesso
				for (size_t j = 0; j < limitStr.size(); j++) {
					if (!std::isdigit(static_cast<unsigned char>(limitStr[j]))) {
						sendNumericReply(client, ERR_INVALIDMODEPARAM,
							channelName + " l " + limitStr + " :Invalid limit value");
						return (false);
					}
				}
				change.param = params[nextParamIdx];
				nextParamIdx++;
			}
			changes.push_back(change);
		}
		else
			sendNumericReply(client, ERR_UNKNOWNMODE, std::string(1, c) + " :is unknown mode char to me");
	}
	return (true);
}

void	Server::applyModeChanges(Client &client, Channel *channel,
	const std::string &channelName, std::vector<ModeChange> &changes)
{
	std::string modeString;             // es. "+it-k" costruita mano a mano
	std::vector<std::string> modeParams; // parametri nell'ordine in cui compaiono
	char lastSign = 0;

	for (size_t i = 0; i < changes.size(); i++)
	{
		ModeChange &change = changes[i];
		std::string displayParam = change.param;

		if (change.letter == 'i')
			channel->setInviteOnly(change.sign == '+');
		else if (change.letter == 't')
			channel->setTopicRestricted(change.sign == '+');
		else if (change.letter == 'k') {
			if (change.sign == '+')
				channel->setKey(change.param);
			else
				channel->removeKey();
		}
		else if (change.letter == 'o') {
			channel->setOperator(change.param, change.sign == '+');
			// change.param è il nick in lowercase (usato per la lookup in _members);
			// per il messaggio broadcastato si preferisce il nick col case originale
			const std::map<std::string, Channel::ChannelMember> &members = channel->getMembers();
			std::map<std::string, Channel::ChannelMember>::const_iterator mIt = members.find(change.param);
			if (mIt != members.end())
				displayParam = mIt->second.client->getDisplayNick();
		}
		else if (change.letter == 'l') {
			if (change.sign == '+') {
				int limit = std::atoi(change.param.c_str());
				channel->setUserLimit(limit);
			}
			else
				channel->removeUserLimit();
		}

		// costruisce la stringa cumulativa
		if (change.sign != lastSign) {
			modeString += change.sign;
			lastSign = change.sign;
		}
		modeString += change.letter;
		if (!displayParam.empty())
			modeParams.push_back(displayParam);
	}

	if (modeString.empty())
		return ;

	std::string modeMsg = client.getPrefix() + " MODE " + channelName + " " + modeString;
	for (size_t i = 0; i < modeParams.size(); i++)
		modeMsg += " " + modeParams[i];
	modeMsg += "\r\n";

	channel->broadcast(modeMsg, NULL);   // a tutto il canale
}

void Server::sendChannelModeIs(Client &client, Channel *channel)
{
	// risposta a una query MODE: stato attuale del canale, non una modifica
	std::string modes = "+";
	std::string modeParams;

	if (channel->isInviteOnly())
		modes += "i";
	if (channel->isTopicRestricted())
		modes += "t";
	if (channel->hasKey()) {
		modes += "k";
		if (!modeParams.empty()) modeParams += " ";
		modeParams += channel->getKey();
	}
	if (channel->hasUserLimit()) {
		modes += "l";
		if (!modeParams.empty()) modeParams += " ";

		// C++98: converti int a string con ostringstream
		std::ostringstream oss;
		oss << channel->getUserLimit();
		modeParams += oss.str();
	}

	std::string reply = channel->getName() + " " + modes;
	if (!modeParams.empty())
		reply += " " + modeParams;

	sendNumericReply(client, RPL_CHANNELMODEIS, reply);

	// RPL_CREATIONTIME: quando il canale è stato creato, non "adesso"
	std::ostringstream oss;
	oss << channel->getCreationTime();
	sendNumericReply(client, RPL_CREATIONTIME, channel->getName() + " " + oss.str());
}