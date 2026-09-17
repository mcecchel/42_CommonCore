PROG = bot
SRCS = main.cpp Bot.cpp
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -g -std=c++98

all: $(PROG)

$(PROG): $(SRCS) Bot.hpp
	@printf '\033[90m[BOT] Compiling %s...\033[0m\n' "$(PROG)"
	@$(CXX) $(CXXFLAGS) $(SRCS) -o $(PROG)
	@printf '\033[32m[BOT] Compilation complete: ./bot/$(PROG)\033[0m\n'

clean:
	@true

fclean: clean
	rm -f $(PROG)

re: fclean all

.PHONY: all clean fclean re