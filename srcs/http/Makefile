SRC= main.cpp request/RequestParser.cpp request/ResourceResolver.cpp request/UploadHandler.cpp ../config/configParser.cpp ../config/configValidate.cpp ../utils/Utils.cpp response/Response.cpp HttpHandler.cpp  
OBJ = $(SRC:.cpp=.o)
NAME = webserver

CC = c++
FLAGS = -Wall -Wextra -Werror
STD = -std=c++98 -fsanitize=address -g

all: $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(FLAGS) $(STD) $(OBJ) -o $(NAME)
%.o:%.cpp ../../includes/Request.hpp
	$(CC) $(FLAGS) $(STD) -c $< -o $@
clean:
	rm -rf $(OBJ)
fclean: clean
	rm -rf $(NAME)
re: fclean all