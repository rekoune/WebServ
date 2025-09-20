HTTP_SRC = srcs/http/request/Request.cpp  srcs/http/response/Response.cpp srcs/http/request/RequestHandler.cpp srcs/http/HttpHandler.cpp srcs/utils/Utils.cpp 
SERVER_SRC = srcs/server/server.cpp 
CONFIG_SRC = srcs/config/configParser.cpp
SRC= ${HTTP_SRC} ${SERVER_SRC} ${CONFIG_SRC} main.cpp
OBJ = $(SRC:.cpp=.o)
NAME = webserv

CC = c++
FLAGS = -Wall -Wextra -Werror
# STD = -std=c++98

all: $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(FLAGS) $(STD) $(OBJ) -o $(NAME)
%.o:%.cpp 
	$(CC) $(FLAGS) $(STD) -c $< -o $@
clean:
	rm -rf $(OBJ)
fclean: clean
	rm -rf $(NAME)
re: fclean all