#include "Client.hpp"

Client::Client(int fd) : _fd(fd) {
	_nickname = "";
	_username = "";
	_inputBuffer = "";
	_outputBuffer = "";
	_passwordOk = false;
	_registered = false;
	erased = false;
	(void)_fd;
}

bool	Client::hasDataToSend() {
	return (!this->_outputBuffer.empty());
}

void	Client::appendToInputBuffer(const std::string& data) {
	this->_inputBuffer.append(data);
}

std::string	Client::readInputBuffer() const {
	return (this->_inputBuffer);
}

void	Client::eraseFromInputBuffer(size_t n) {
	if (n > this->_inputBuffer.size())
		this->_inputBuffer.clear();// Rimuove tutto se n supera dimensione del buffer
	else
		this->_inputBuffer.erase(0, n);// Rimuove i primi n caratteri dal buffer
}

void	Client::appendToOutputBuffer(const std::string& data) {
	if (!_displayNick.empty())
		std::cout << "Sending to " << _displayNick << ": " << data;
	else
		std::cout << "Sending to unregistered client (fd: " << _fd << "): " << data;
	this->_outputBuffer.append(data);
}

std::string	Client::readOutputBuffer() const {
	return (this->_outputBuffer);
}

void	Client::eraseFromOutputBuffer(size_t n) {
	if (n > this->_outputBuffer.size())
		this->_outputBuffer.clear();// Rimuove tutto se n supera dimensione del buffer
	else
		this->_outputBuffer.erase(0, n);// Rimuove i primi n caratteri dal buffer
}

bool	Client::hasToBeRemoved() {
	return (this->erased);
}

bool	Client::isRegistered() {
	return (_registered);
}

void	Client::markForRemoval() {          // NUOVO
	this->erased = true;
}

bool	Client::getPasswordStatus() {
	return (_passwordOk);
}

void	Client::setPasswordOk() {
	this->_passwordOk = true;
}

void	Client::setNickname(const std::string& nick) {
	_displayNick = nick;
	_nickname = nick;
	for (std::size_t i = 0; i < _nickname.size(); ++i)
		_nickname[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(_nickname[i])));
}

std::string	Client::getNickname() const {
	return (_nickname);
}
std::string Client::getDisplayNick() const {
	return (_displayNick);
}

void	Client::setUsername(const std::string& user) {
	_username = user;
}

std::string	Client::getUsername() const {
	return (_username);
}

void	Client::setRealname(const std::string& str) {
	_realname = str;
}

std::string	Client::getRealname() const {
	return (_realname);
}

int	Client::getFd() const {
	return (_fd);
}

bool	Client::updateRegistrationStatus() {
	if(isRegistered())
		return (false);

	if (!getNickname().empty() && !getUsername().empty() && getPasswordStatus()) {
		_registered = true;
		return (true);
	}
	return (false);
}

std::string Client::getPrefix() const {
	return (":" + getDisplayNick() + "!" + getUsername() + "@localhost");
}

void Client::addPmContact(const std::string &nick) {
	_pmContacts.insert(nick);
}

void Client::renamePmContact(const std::string &oldNick,
	const std::string &newNick) {
	std::set<std::string>::iterator it = _pmContacts.begin();

	while (it != _pmContacts.end()) {
		if (it->size() == oldNick.size()) {
			bool equal = true;

			for (std::size_t i = 0; i < oldNick.size(); ++i) {
				if (std::tolower(static_cast<unsigned char>((*it)[i]))
					!= std::tolower(static_cast<unsigned char>(oldNick[i]))) {
					equal = false;
					break;
				}
			}

			if (equal) {
				_pmContacts.erase(it);
				_pmContacts.insert(newNick);
				return;
			}
		}
		++it;
	}
}

const std::set<std::string> &Client::getPmContacts() const {
	return (_pmContacts);
}