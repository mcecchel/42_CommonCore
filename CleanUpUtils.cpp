#include "Server.hpp"

void	Server::removeClient(int fd) {
	std::map<int, Client*>::iterator it = clients.find(fd);

	if (it != clients.end() && it->second != NULL) {
		Client	*client = it->second;
		if (!client->getDisplayNick().empty())
			std::cout << "Removed user: " << client->getDisplayNick() << std::endl ;
		else
			std::cout << "Removed unregistered client fd: " << fd << std::endl;

		// Rimuove il client da tutti i canali a cui apparteneva,
		// avvisando gli altri membri, prima di distruggerlo.
		std::vector<std::string> clientChannels = getClientChannels(*client);
		std::string quitMsg = client->getPrefix() + " QUIT :Connection closed\r\n";

		for (size_t i = 0; i < clientChannels.size(); i++) {
			std::map<std::string, Channel*>::iterator chIt = channels.find(clientChannels[i]);
			if (chIt == channels.end())
				continue ;
			Channel *channel = chIt->second;
			channel->broadcast(quitMsg, client); // il client disconnesso non riceve nulla
			channel->removeMember(client->getNickname());
			if (channel->memberCount() == 0) {
				delete channel;
				channels.erase(chIt);
			}
			else
				promoteOperatorIfNeeded(channel);
		}
		delete client;
	}
	clients.erase(fd);

	close(fd);

	// Rimuove il pollfd corrispondente dal vettore
	for (size_t i = 0; i < fds.size(); i++) {
		if (fds[i].fd == fd) {
			fds.erase(fds.begin() + i);
			break ;
		}
	}
}

void	Server::cleanupClients() {
	size_t idx = 1; // idx 0 e' il socket del server, non va mai toccato

	while (idx < fds.size()) {
		int fd = fds[idx].fd;
		std::map<int, Client*>::iterator it = clients.find(fd);
		bool unknown = (it == clients.end() || it->second == NULL);
		bool doneRemoving = !unknown && it->second->hasToBeRemoved();

		if (unknown || doneRemoving)
			removeClient(fd); // dopo l'erase, in idx c'e' l'elemento successivo: non incrementare
		else
			idx++;
	}
}

void	Server::serverDestroyer() {
	std::map<int, Client*>::iterator cIt = clients.begin();
	while (cIt != clients.end()) {
		if (cIt->second != NULL) {
			close(cIt->first);
			delete cIt->second;
		}
		++cIt;
	}
	clients.clear();

	std::map<std::string, Channel*>::iterator chIt = channels.begin();
	while (chIt != channels.end()) {
		delete chIt->second;
		++chIt;
	}
	channels.clear();

	if (server_fd != -1) {
		close(server_fd);
		server_fd = -1;
	}
	fds.clear();
}
