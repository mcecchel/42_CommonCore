#include "Channel.hpp"

static std::string toLowerCase(const std::string &str) {
	std::string result = str;

	for (size_t i = 0; i < result.size(); i++)
		result[i] = std::tolower(static_cast<unsigned char>(result[i]));
	return (result);
}

Channel::Channel(const std::string &name) {
	_name = toLowerCase(name);
	_creationTime = time(NULL);
	_topic = "";
	_topicSetBy = "";
	_topicSetTime = 0;
	_inviteOnly = false;
	_topicRestricted = false;
	_hasKey = false;
	_key = "";
	_hasUserLimit = false;
	_userLimit = 0;
}

const std::map<std::string, Channel::ChannelMember>& Channel::getMembers() const {
return (_members);
}

const std::string &Channel::getName() const {
	return (_name);
}

void Channel::addMember(Client *client, bool asOperator) {
	ChannelMember member;
	member.client = client;
	member.isOperator = asOperator;
	std::cout << "[DEBUG] added member with key: '" << client->getNickname() << "'" << std::endl;
	_members[client->getNickname()] = member;
}

void Channel::removeMember(const std::string &nickname) {
	_members.erase(nickname);
	_invited.erase(nickname);
}

bool Channel::isMember(const std::string &nickname) const {
	return (_members.find(nickname) != _members.end());
}

bool Channel::isOperator(const std::string &nickname) const {
	std::map<std::string, ChannelMember>::const_iterator it = _members.find(nickname);
	if (it == _members.end())
		return (false);
	return (it->second.isOperator);
}

void Channel::setOperator(const std::string &nickname, bool value) {
	std::map<std::string, ChannelMember>::iterator it = _members.find(nickname);
	if (it != _members.end())
		it->second.isOperator = value;
}

bool Channel::hasOperators() const {
	std::map<std::string, ChannelMember>::const_iterator it;
	for (it = _members.begin(); it != _members.end(); ++it) {
		if (it->second.isOperator)
			return (true);
	}
	return (false);
}

size_t Channel::memberCount() const {
	return (_members.size());
}

// NUOVO: aggiorna la chiave della mappa quando un membro cambia nickname
void Channel::renameMember(const std::string &oldNick, const std::string &newNick) {
	std::map<std::string, ChannelMember>::iterator it = _members.find(oldNick);
	if (it == _members.end())
		return ;
	ChannelMember member = it->second;
	_members.erase(it);
	_members[newNick] = member;
	if (_invited.erase(oldNick))
		_invited.insert(newNick);
}

const std::string &Channel::getTopic() const {
	return (_topic);
}

void Channel::setTopic(const std::string &topic) {
	_topic = topic;
}

const std::string &Channel::getTopicSetBy() const {
	return (_topicSetBy);
}

time_t Channel::getTopicSetTime() const {
	return (_topicSetTime);
}

time_t Channel::getCreationTime() const {
	return (_creationTime);
}

void Channel::setTopicMetadata(const std::string &nickname, time_t timestamp) {
	_topicSetBy = nickname;
	_topicSetTime = timestamp;
}

bool Channel::isInviteOnly() const {
	return (_inviteOnly);
}

void Channel::setInviteOnly(bool value) {
	_inviteOnly = value;
}

bool Channel::isTopicRestricted() const {
	return (_topicRestricted);
}

void Channel::setTopicRestricted(bool value) {
	_topicRestricted = value;
}

const std::string &Channel::getKey() const {
	return (_key);
}

void Channel::setKey(const std::string &key) {
	_key = key;
	_hasKey = true;
}

void	Channel::removeKey() {
		_key = "";
		_hasKey = false;
}

bool Channel::hasKey() const {
	return _hasKey;
}

int Channel::getUserLimit() const {
	return _userLimit;
}

void Channel::setUserLimit(int limit) {
	_userLimit = limit;
	_hasUserLimit = true;
}

void Channel::removeUserLimit() {
	_userLimit = 0;
	_hasUserLimit = false;
}

bool Channel::hasUserLimit() const {
	return (_hasUserLimit);
}

void	Channel::addInvite(const std::string &nickname) {
	_invited.insert(nickname);
}

bool	Channel::isInvited(const std::string &nickname) const
{
	return (_invited.find(nickname) != _invited.end());
}

void	Channel::removeInvite(const std::string &nickname) {
	_invited.erase(nickname);
}

void	Channel::broadcast(const std::string &message, Client *exclude) {
	std::map<std::string, ChannelMember>::iterator it;
	
	for (it = _members.begin(); it != _members.end(); ++it) {
		Client *client = it->second.client;
		if (client != exclude && !client->hasToBeRemoved())
			client->appendToOutputBuffer(message);
	}
}