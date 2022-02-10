# AUTHOR:	@AYoungSn @mocha-kim
# PUBLISH:	2022/02

NAME = webserv

SRCS_PATH		= ./srcs
INCS_PATH		= ./includes

SRCS			= $(SRCS_PATH)/main.cpp \
					$(SRCS_PATH)/Server.cpp $(SRCS_PATH)/Location.cpp \
					$(SRCS_PATH)/Request.cpp $(SRCS_PATH)/Response.cpp \
					$(SRCS_PATH)/ServerManager.cpp

OBJS			= $(SRCS:.cpp=.o)

CXX				= clang++
CXXFLAGS		= -Wall -Wextra -Werror

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
				rm -f $(OBJS)

fclean:			clean
				rm -f $(NAME)

re:				fclean $(NAME)

.PHONY:			all clean fclean re