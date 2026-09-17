#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Server.hpp"
# include <set>

class Client;

class Channel {
	public:
		struct ChannelMember {
				Client	*client;
				bool	isOperator;
			};
	private:
		std::string								_name;
		std::map<std::string, ChannelMember>	_members;   // nickname -> {Client*, isOperator}
		time_t									_creationTime;
		std::string								_topic;
		std::string								_topicSetBy;
		time_t									_topicSetTime;
		bool									_inviteOnly;
		bool									_topicRestricted;
		bool									_hasKey;
		std::string								_key;
		bool									_hasUserLimit;
		int										_userLimit;
		std::set<std::string>					_invited;   // nickname degli invitati

	public:
		Channel(const std::string &name);
		const std::map<std::string, ChannelMember>& getMembers() const;

		const std::string &getName() const;

		time_t		getCreationTime() const;

		void		addMember(Client *client, bool asOperator = false);
		void		removeMember(const std::string &nickname);
		bool		isMember(const std::string &nickname) const;

		void		renameMember(const std::string &oldNick, const std::string &newNick);
		
		bool		isOperator(const std::string &nickname) const;
		void		setOperator(const std::string &nickname, bool value);
		bool		hasOperators() const;

		size_t		memberCount() const;

		const		std::string &getTopic() const;
		void		setTopic(const std::string &topic);
		const	std::string &getTopicSetBy() const;
		time_t		getTopicSetTime() const;
		void		setTopicMetadata(const std::string &nickname, time_t timestamp);

		bool		isInviteOnly() const;
		void		setInviteOnly(bool value);

		bool		isTopicRestricted() const;
		void		setTopicRestricted(bool value);

		const std::string	&getKey() const;
		void				setKey(const std::string &key);
		void				removeKey();
		bool				hasKey() const;

		int			getUserLimit() const;
		void		setUserLimit(int limit);
		void		removeUserLimit();
		bool		hasUserLimit() const;

		void		addInvite(const std::string &nickname);
		bool		isInvited(const std::string &nickname) const;
		void		removeInvite(const std::string &nickname);   // dopo il JOIN, l'invito va "consumato"

		void		broadcast(const std::string &message, Client *exclude = NULL); 

};

#endif