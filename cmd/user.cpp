#include "../Server.hpp"

void	Server::handleUser(Client& client, std::vector<std::string>& params) {
if (client.isRegistered()) {
		sendNumericReply(client, ERR_ALREADYREGISTRED, ":You may not register");
		return ;
	}

	if (params.size() < 4 || params[0].empty()){
		sendNumericReply(client, ERR_NEEDMOREPARAMS , ":Erroneous number of params. Insert *username 0 * : realusername");
		return ;
	}

	client.setUsername(params[0]);
	if (params[3][0] == ':')
		params[3] = params[3].substr(1);
	client.setRealname(params[3]);
	if(client.updateRegistrationStatus())
		sendWelcomeSequence(client);

}