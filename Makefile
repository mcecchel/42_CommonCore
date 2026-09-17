# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mbrighi <mbrighi@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/07/08 11:07:37 by mbrighi           #+#    #+#              #
#    Updated: 2026/09/03 15:29:36 by mbrighi          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

PROG	= irc

SRCS 	= main.cpp Server.cpp ServerLoop.cpp ServerUtils.cpp \
			handleParsing.cpp Client.cpp Channel.cpp CleanUpUtils.cpp\
			cmd/join.cpp cmd/mode.cpp cmd/nick.cpp \
			cmd/part.cpp cmd/privmsg.cpp cmd/pass.cpp \
			cmd/user.cpp cmd/topic.cpp cmd/quit.cpp cmd/kick.cpp \
			cmd/invite.cpp
HEADER	= -I.

CXX 	= c++
CXXFLAGS 	= -Wall -Wextra -Werror -g -v -std=c++98
CXXFLAGS 	= -Wall -Wextra -Werror -g -std=c++98

all: 		${PROG}
all:		${PROG}

bot:
					@$(MAKE) --no-print-directory -C bot

${PROG}:	${SRCS}
					@printf '\033[90m[IRC] Compiling %s...\033[0m\n' "${PROG}"
					@$(CXX) ${SRCS} ${CXXFLAGS} ${HEADER} -o ${PROG}
					$(MAKE) yolo
					@printf '\033[32m[IRC] Compilation complete: ./%s\033[0m\n' "${PROG}"

clean:
					@echo "Nothing to clean (no .o files)"

fclean: clean
					@rm -f $(PROG)
					@$(MAKE) --no-print-directory -C bot fclean

re:			fclean all

yolo:
	@clear
	@echo	'⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠀⠠'
	@echo	'⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠶⡣⠑⠁'
	@echo	'⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⢤⣮⠙⠓⠈⠀⠀'
	@echo	'⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡴⢊⡵⠋⠀⠀⠀⠀⠀⠀'
	@echo	'⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⢴⡯⠚⠁⠀⠀⠀⠀⠀⠀⠀⠀'
	@echo	'⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⡾⠛⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀'
	@echo	'⠀⢀⣤⣶⣿⣿⣷⣦⡀⠀⠀⠀⢀⣠⡾⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀'
	@echo	'⣰⣿⣿⣿⣿⣿⣿⣿⣿⣆⠀⢶⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀'
	@echo	'⣿⣿⣿⣿⣿⣿⣿⣿⣿⣟⢀⠔⢔⠊⡠⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀'
	@echo	'⢻⣿⣿⣿⣿⣿⡟⠉⢙⢟⢵⢿⠗⢨⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀'
	@echo	'⠀⠙⣿⠛⠙⢏⣑⠖⡱⣮⠄⠁⢀⡾⢀⡇⢰⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀'
	@echo	'⠀⠀⠘⠓⠒⢉⡵⣊⡴⣃⣠⠄⠉⣠⠞⢡⡟⢠⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀'
	@echo	'⠀⢀⠖⢠⢔⣡⠞⠋⠀⠐⠶⡖⣀⡀⠘⠩⠴⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀'
	@echo	'⠀⠃⢺⡿⠋⠁⠀⠀⠀⠀⠀⠈⠁⠴⢄⠐⠧⣠⣞⡻⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀'
	@echo	'⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⡾⠟⠈⣿⡿⠃⠺⠷⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀'
	@echo	'⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⠏⠀⠀⠀⠀⠀⠀⠀⠀⠈⠳⣄⠀⠀⠀⠀⠀⠀⠀'
	@echo	'⠀⠀⠀⠀⠀⠀⠀⠀⣠⡿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢷⣄⠀⠀⠀⠀⠀'
	@echo	'⠀⠀⠀⠀⠀⠀⠐⢿⡋⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠿⣛⡃⠀⠀⠀'
	@echo	'⠀⠀⠀⠀⠀⠀⠀⣩⣥⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣯⠆⠀⠀⠀'
	@echo	'⠀⠀⠀⠀⠀⠀⠀⠈⢻⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⡟⠀⠀⠀⠀'
	@echo	'⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⠁⠀⠀⠀⠀'
	@echo	'⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⡇⠀⠀⠀⠀⠀'
	@echo	'⠀⠀⠀⠀⠀⠀⢀⠀⣤⢲⣦⣤⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⡶⡶⣰⠤⠀⠀⠀⠀'
	@echo	'⠀⠀⠀⠀⠀⠀⠀⠛⠉⠁⠁⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠉⠒⠁⠁⠀⠀⠀'

.PHONY: all bot clean fclean re re_bonus


