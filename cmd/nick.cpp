#include "../Server.hpp"

bool	Server::isValidNickname(const std::string& str) {
	if (str.empty() || str.size() > 20)
		return (false);

	if (str.find(' ') != std::string::npos || str.find(':') != std::string::npos)
		return (false);

	unsigned char first = static_cast<unsigned char>(str[0]);
	if (!std::isalpha(first) && str[0] != '[' && str[0] != ']' && str[0] != '\''
		 && str[0] != '_' &&  str[0] != '^' && str[0] != '{' && str[0] != '}'
		 && str[0] != '|')
			return (false);
	for (std::size_t i = 0; i < str.size(); i++) {
		unsigned char x = static_cast<unsigned char>(str[i]);
		if (!std::isalpha(x) && !std::isdigit(x) && str[i] != '[' && str[i] != ']'
		 && str[i] != '\'' && str[i] != '_' &&  str[i] != '^' && str[i] != '{'
		 && str[i] != '}' && str[i] != '|' && str[i] != '-')
		 	return(false);
	}
	return (true);
}

bool	Server::isNicknameTaken(const std::string& str, const Client *self) {
	std::map<int, Client*>::const_iterator it;
	for (it = this->clients.begin(); it != clients.end(); ++it) {
		if (it->second == self)
			continue ; // salta il client che sta facendo la richiesta
		if (ircEqualString(it->second->getNickname(), str))
			return (true);
	}
	return (false);
}

void Server::handleNick(Client &client, std::vector<std::string> &params) {
	if (params.empty() || params[0].empty()) {
		sendNumericReply(client, ERR_NONICKNAMEGIVEN, ":No nickname given");
		return;
	}

	if (!isValidNickname(params[0])) {
		sendNumericReply(client, ERR_ERRONEUSNICKNAME, params[0] + " :Erroneous nickname");
		return;
	}

	if (isNicknameTaken(params[0], &client)) {
		sendNumericReply(client, ERR_NICKNAMEINUSE, params[0] +  " :Nickname already in use");
		return;
	}

	std::string oldDisplay = client.getDisplayNick();
	std::string oldNick = client.getNickname();
	client.setNickname(params[0]);
	std::string newNick = client.getNickname();

	if (!oldNick.empty() && oldNick != newNick) {
		std::map<std::string, Channel*>::iterator it;
		for (it = channels.begin(); it != channels.end(); ++it) {
			if (it->second->isMember(oldNick))
				it->second->renameMember(oldNick, newNick);
		}
	}
	
	if (client.updateRegistrationStatus()) {
		sendWelcomeSequence(client);
		return;
	}

		// Messaggio NICK standard IRC
	if (!oldNick.empty() && oldNick != newNick) {
		std::string notice = ":" + oldDisplay + "!" + client.getUsername() 
							+ "@localhost NICK :" + params[0] + "\r\n";
		client.appendToOutputBuffer(notice);

		// Un solo invio per destinatario, anche se condivide più canali
		// o sia un canale sia una PM diretta con chi ha cambiato nick.
		std::set<Client*> recipients;

		std::map<std::string, Channel*>::iterator chIt;
		for (chIt = channels.begin(); chIt != channels.end(); ++chIt) {
			if (chIt->second->isMember(client.getNickname())) {
				const std::map<std::string, Channel::ChannelMember> &members = chIt->second->getMembers();
				std::map<std::string, Channel::ChannelMember>::const_iterator mIt;
				for (mIt = members.begin(); mIt != members.end(); ++mIt)
					if (mIt->second.client != &client)
						recipients.insert(mIt->second.client);
			}
		}

		const std::set<std::string> &contacts = client.getPmContacts();
		std::set<std::string>::const_iterator pIt;
		for (pIt = contacts.begin(); pIt != contacts.end(); ++pIt) {
			std::map<int, Client*>::iterator cIt;
			for (cIt = clients.begin(); cIt != clients.end(); ++cIt) {
				if (cIt->second != NULL && ircEqualString(cIt->second->getNickname(), *pIt)) {
					recipients.insert(cIt->second);
					cIt->second->renamePmContact(oldNick, newNick);   // aggiorna il loro contatto col nuovo nick
					break ;
				}
			}
		}

		std::set<Client*>::iterator rIt;
		for (rIt = recipients.begin(); rIt != recipients.end(); ++rIt)
			(*rIt)->appendToOutputBuffer(notice);
	}
}