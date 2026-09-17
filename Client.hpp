#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"
# include <set>

class Client {
	public:
		Client(int fd);
		bool		hasDataToSend();

		void		appendToInputBuffer(const std::string& data);// Accumula byte grezzi
		std::string	readInputBuffer() const;// Legge contenuto attuale
		void		eraseFromInputBuffer(size_t n);// Rimuove parte gia' consumata

		void		appendToOutputBuffer(const std::string& data);// Accumula byte grezzi
		std::string	readOutputBuffer() const;// Legge contenuto attuale
		void		eraseFromOutputBuffer(size_t n);// Rimuove parte gia' consumata

		bool		hasToBeRemoved();
		bool		isRegistered();
		void		markForRemoval();   // NUOVO

		// Metodi per la gestione della registrazione del client
		bool		getPasswordStatus();
		void		setPasswordOk();

		void		setNickname(const std::string& nick);
		std::string	getNickname() const;
		std::string	getDisplayNick() const;
		void 		setRealname(const std::string& str);
		std::string	getRealname() const;

		void		setUsername(const std::string& user);
		std::string	getUsername() const;

		int			getFd() const;

		bool		updateRegistrationStatus();

		std::string	getPrefix() const;

	void						addPmContact(const std::string &nick);
	void						renamePmContact(const std::string &oldNick, const std::string &newNick);
	const std::set<std::string> &getPmContacts() const;

	private:
		std::string		_displayNick;
		int				_fd;
		std::string		_nickname;
		std::string		_username;
		std::string		_realname;
		
		bool			_passwordOk;
		std::string		_inputBuffer;
		std::string		_outputBuffer;

		bool			_registered;
		bool			erased;

		std::set<std::string>	_pmContacts;
};

#endif