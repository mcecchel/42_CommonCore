
#ifndef NUMERIC_REPLIES_HPP
# define NUMERIC_REPLIES_HPP

enum NumericReply {
    // ============================================================
    // COMANDI DI REGISTRAZIONE E CONNESSIONE
    // ============================================================
    RPL_WELCOME           = 001,  // ":Welcome to the Internet Relay Network <nick>"
    RPL_YOURHOST          = 002,  // ":Your host is <servername>, running version <ver>"
    RPL_CREATED           = 003,  // ":This server was created <date>"
    RPL_MYINFO            = 004,  // "<servername> <version> <user modes> <channel modes>"
    RPL_BOUNCE            = 005,  // ":Try server <server>, port <port>"
    
    // ============================================================
    // COMANDO NICK
    // ============================================================
    ERR_INPUTTOOLONG      = 414,  // ":Input line too long"
    ERR_NONICKNAMEGIVEN   = 431,  // ":No nickname given"
    ERR_ERRONEUSNICKNAME  = 432,  // "<nick> :Erroneous nickname"
    ERR_NICKNAMEINUSE     = 433,  // "<nick> :Nickname is already in use"
    ERR_NICKCOLLISION     = 436,  // "<nick> :Nickname collision KILL"
    ERR_UNAVAILRESOURCE   = 437,  // "<nick> :Nick/channel is temporarily unavailable"
    
    // ============================================================
    // COMANDI USER E REGISTRAZIONE
    // ============================================================
    ERR_NEEDMOREPARAMS    = 461,  // "<command> :Not enough parameters"
    ERR_ALREADYREGISTRED  = 462,  // ":You may not reregister"
    ERR_PASSWDMISMATCH    = 464,  // ":Password incorrect"
    ERR_NOTREGISTERED     = 451,  // ":You have not registered"
    
    // ============================================================
    // COMANDO PRIVMSG / NOTICE
    // ============================================================
    ERR_NOSUCHNICK        = 401,  // "<nickname> :No such nick/channel"
    ERR_NOSUCHSERVER      = 402,  // "<server name> :No such server"
    ERR_CANNOTSENDTOCHAN  = 404,  // "<channel name> :Cannot send to channel"
    ERR_TOOMANYTARGETS    = 407,  // "<target> :Duplicate recipients. No message delivered"
    ERR_NORECIPIENT       = 411,  // ":No recipient given (<command>)"
    ERR_NOTEXTTOSEND      = 412,  // ":No text to send"
    ERR_NOTOPLEVEL        = 413,  // "<mask> :No toplevel domain specified"
    ERR_WILDTOPLEVEL      = 414,  // "<mask> :Wildcard in toplevel domain"
    
    // ============================================================
    // COMANDO JOIN
    // ============================================================
    ERR_BADCHANNELKEY     = 475,  // "<channel> :Cannot join channel (+k)"
    ERR_CHANNELISFULL     = 471,  // "<channel> :Cannot join channel (+l)"
    ERR_INVITEONLYCHAN    = 473,  // "<channel> :Cannot join channel (+i)"
    ERR_BANNEDFROMCHAN    = 474,  // "<channel> :Cannot join channel (+b)"
    ERR_TOOMANYCHANNELS   = 405,  // "<channel> :You have joined too many channels"
    ERR_NOSUCHCHANNEL     = 403,  // "<channel> :No such channel"
    
    // ============================================================
    // COMANDO TOPIC
    // ============================================================
    RPL_TOPIC             = 332,  // "<channel> :<topic>"
    RPL_TOPICWHOTIME      = 333,  // "<channel> <nick> <time>"
    ERR_NOTOPIC           = 331,  // "<channel> :No topic is set"
    
    // ============================================================
    // COMANDO NAMES
    // ============================================================
    RPL_NAMREPLY          = 353,  // "<channel> :[[@|+]<nick> [[@|+]<nick> ...]]"
    RPL_ENDOFNAMES        = 366,  // "<channel> :End of NAMES list"
    
    // ============================================================
    // COMANDO LIST
    // ============================================================
    RPL_LISTSTART         = 321,  // "Channel :Users Name"
    RPL_LIST              = 322,  // "<channel> <#visible> :<topic>"
    RPL_LISTEND           = 323,  // ":End of LIST"
    
    // ============================================================
    // COMANDO WHO / WHOIS
    // ============================================================
    RPL_WHOREPLY          = 352,  // "<channel> <user> <host> <server> <nick> <flags> :<hopcount> <realname>"
    RPL_ENDOFWHO          = 315,  // "<name> :End of WHO list"
    RPL_WHOISUSER         = 311,  // "<nick> <user> <host> * :<realname>"
    RPL_WHOISSERVER       = 312,  // "<nick> <server> :<server info>"
    RPL_WHOISOPERATOR     = 313,  // "<nick> :is an IRC operator"
    RPL_WHOISIDLE         = 317,  // "<nick> <seconds> :seconds idle"
    RPL_ENDOFWHOIS        = 318,  // "<nick> :End of WHOIS list"
    RPL_WHOISCHANNELS     = 319,  // "<nick> :<channel list>"
    ERR_NOSUCHNICK_WHOIS  = 401,  // "<nickname> :No such nick/channel"
    ERR_NOSUCHSERVER_WHOIS= 402,  // "<server name> :No such server"
    
    // ============================================================
    // COMANDO PART
    // ============================================================
    ERR_NOTONCHANNEL      = 442,  // "<channel> :You're not on that channel"
    
    // ============================================================
    // COMANDO KICK
    // ============================================================
    ERR_USERNOTINCHANNEL  = 441,  // "<nick> <channel> :They aren't on that channel"
    ERR_CHANOPRIVSNEEDED  = 482,  // "<channel> :You're not channel operator"
    
    // ============================================================
    // COMANDO INVITE
    // ============================================================
    RPL_INVITING          = 341,  // "<channel> <nick>"
    ERR_USERONCHANNEL     = 443,  // "<user> <channel> :is already on channel"
    ERR_NOTONCHANNEL_INVITE = 442, // "<channel> :You're not on that channel"
    
    // ============================================================
    // COMANDO MODE (canale)
    // ============================================================
    RPL_CHANNELMODEIS     = 324,  // "<channel> <mode> <mode params>"
    RPL_CREATIONTIME      = 329,  // "<channel> <time>"
    ERR_UNKNOWNMODE       = 472,  // "<char> :is unknown mode char to me"
    ERR_KEYSET            = 467,  // "<channel> :Channel key already set"
    ERR_INVALIDMODEPARAM  = 696,  // "<target> <mode> <param> :<description>"
    
    // ============================================================
    // COMANDO MODE (utente)
    // ============================================================
    ERR_UMODEUNKNOWNFLAG  = 501,  // ":Unknown MODE flag"
    ERR_USERSDONTMATCH    = 502,  // ":Cant change mode for other users"
    
    // ============================================================
    // COMANDO OPER
    // ============================================================
    RPL_YOUREOPER         = 381,  // ":You are now an IRC operator"
    ERR_NOOPERHOST        = 491,  // ":No O-lines for your host"
    ERR_NOPRIVILEGES      = 481,  // ":Permission Denied- You're not an IRC operator"
    
    // ============================================================
    // COMANDO QUIT
    // ============================================================
    // Nessun codice numerico specifico per QUIT (è una notifica)
    
    // ============================================================
    // PARSING GENERALE
    // ============================================================
    ERR_UNKNOWNCOMMAND    = 421,  // "<command> :Unknown command"
    
    // ============================================================
    // SERVER / MOTD
    // ============================================================
    RPL_MOTDSTART         = 375,  // ":- <server> Message of the day - "
    RPL_MOTD              = 372,  // ":- <text>"
    RPL_ENDOFMOTD         = 376,  // ":End of MOTD command"
    ERR_NOMOTD            = 422,  // ":MOTD File is missing"
    
    // ============================================================
    // PING / PONG
    // ============================================================
    ERR_NOORIGIN          = 409,  // ":No origin specified"
    
    // ============================================================
    // BAN (MODALITA' +b)
    // ============================================================
    RPL_BANLIST           = 367,  // "<channel> <banmask>"
    RPL_ENDOFBANLIST      = 368,  // "<channel> :End of channel ban list"
    
    // ============================================================
    // ERRORI VARI
    // ============================================================
    ERR_ALREADYOPER       = 484,  // ":Already an operator"
    ERR_OPERONCHANNEL     = 485,  // ":Cannot join channel (oper-only)"
};

#endif