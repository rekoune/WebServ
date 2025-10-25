HTTP_SRC = srcs/http/request/RequestParser.cpp srcs/http/request/ResourceResolver.cpp srcs/http/request/UploadHandler.cpp srcs/http/HttpHandler.cpp srcs/http/response/Response.cpp srcs/http/response/GetHandler.cpp srcs/utils/Utils.cpp 
SERVER_SRC = srcs/server/server.cpp  srcs/server/client.cpp 
CONFIG_SRC = srcs/config/configParser.cpp srcs/config/configValidate.cpp
HEADER_SRC = includes/client.hpp includes/server.hpp  includes/RequestParser.hpp includes/ResourceResolver.hpp includes/UploadHandler.hpp includes/HttpHandler.hpp includes/Response.hpp includes/GetHandler.hpp includes/Utils.hpp
SRC= ${HTTP_SRC} ${SERVER_SRC} ${CONFIG_SRC} main.cpp
OBJ = $(SRC:.cpp=.o)
NAME = webserv

CC = c++
FLAGS = -Wall -Wextra -Werror
STD = -std=c++98 -fsanitize=address -g

all: $(NAME) 

$(NAME) : $(OBJ) $(HEADER_SRC)
	$(CC) $(FLAGS) $(STD) $(OBJ) -o $(NAME)
%.o:%.cpp 
	$(CC) $(FLAGS) $(STD) -c $< -o $@
clean:
	rm -rf $(OBJ)
fclean: clean
	rm -rf $(NAME)
re: fclean all

install:
	cp $(NAME) ~/goinfre/PROJECT/
