#include "../Server.hpp"

void Server::validatePassword(Client& client, std::vector<std::string>& params) {
	// 1. Controllo parametri
	if (params.empty()) {
		sendNumericReply(client, ERR_NEEDMOREPARAMS, "PASS :Not enough parameters");
		return;
	}
	// 2. Già registrato? → RIFIUTA SUBITO (senza controllare la password)
	if (client.isRegistered()) {
		sendNumericReply(client, ERR_ALREADYREGISTRED, ":You may not reregister");
		return;
	}
	// 3. Solo ora controlla la password
	if (params[0] != _psw) {
		sendNumericReply(client, ERR_PASSWDMISMATCH, ":Password incorrect");
		return;
	}
	// 4. Password corretta
	client.setPasswordOk();
	if(client.updateRegistrationStatus())
		sendWelcomeSequence(client);
}