#include "Server.hpp"

void	Server::handleParsing(Client &client, const std::string &line)
{
	if (line.empty())
		return ;
	if (line.size() > 512)
	{
		sendNumericReply(client, ERR_INPUTTOOLONG, ":Input line too long");
		return ;
	}
	if (!client.getDisplayNick().empty())
		std::cout << "Received from " << client.getDisplayNick() << ": " << line << std::endl;
	else
		std::cout << "Received from unregistered client (fd: " << client.getFd() << "): " << line << std::endl;
	std::string command;
	std::vector<std::string> params;
	size_t	pos = 0;
	size_t	len = line.length();
	while (pos < len && line[pos] == ' ')
		pos++;
	size_t cmdStart = pos;
	while (pos < len && line[pos] != ' ')
		pos++;
	command = line.substr(cmdStart, pos - cmdStart);
	for (size_t i = 0; i < command.size(); i++) {
		command[i] = std::toupper(static_cast<unsigned char>(command[i]));
	}
	while (pos < len)
	{
		while (pos < len && line[pos] == ' ')
			pos++;
		if (pos >= len)
	   		break;
		if (pos < len && line[pos] == ':')
		{
			params.push_back(line.substr(pos + 1));
			break;
		}
		cmdStart = pos;
		while (pos < len && line[pos] != ' ')
			pos++;
		params.push_back(line.substr(cmdStart, pos - cmdStart));
	}
	
	// Gate di registrazione
	if (!client.isRegistered() && command != "PASS" && command != "NICK"
		&& command != "USER" && command != "QUIT" && command != "PING") {
		sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return ;
	}
	if (command == "PASS")
		validatePassword(client, params);
	else if (command == "NICK")
		handleNick(client, params);// TODO: - da aggiornare firma (Step 6)
	else if (command == "USER")
		handleUser(client, params);// TODO: - da aggiornare firma (Step 6)
	else if (command == "PING")
		handlePing(client, params);
	else if (command == "JOIN")
		handleJoin(client, params);
	else if (command == "PRIVMSG")
		handlePrivMsg(client, params);
	else if (command == "KICK")
		handleKick(client, params);
	else if (command == "INVITE")
		handleInvite(client, params);
	else if (command == "TOPIC")
		handleTopic(client, params);
	else if (command == "MODE")
		handleMode(client, params);
	else if (command == "QUIT")
		handleQuit(client, params);
	else if (command == "PART")
		handlePart(client, params);
	else
		sendNumericReply(client, ERR_UNKNOWNCOMMAND, command + " :Unknown command");
}

