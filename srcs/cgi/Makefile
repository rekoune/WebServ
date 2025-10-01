HTTP_SRC = ../http/request/RequestParser.cpp ../http/request/ResourceResolver.cpp ../http/request/UploadHandler.cpp ../http/HttpHandler.cpp ../http/response/Response.cpp ../utils/Utils.cpp 
SERVER_SRC = ../server/server.cpp  ../server/client.cpp 
CONFIG_SRC = ../config/configParser.cpp ../config/configValidate.cpp
CGI_SRC = CgiExecutor.cpp 
SRC= ${HTTP_SRC} ${SERVER_SRC} ${CONFIG_SRC} ${CGI_SRC} main.cpp
OBJ = $(SRC:.cpp=.o)
NAME = webserv

CC = c++
FLAGS = -Wall -Wextra -Werror
STD = -std=c++98 -fsanitize=address -g

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