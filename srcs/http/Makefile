SRC= main.cpp request/Request.cpp  response/Response.cpp request/RequestHandler.cpp HttpHandler.cpp ../utils/Utils.cpp ../config/configParser.cpp
OBJ = $(SRC:.cpp=.o)
NAME = Request

CC = c++
FLAGS = -Wall -Wextra -Werror
# STD = -std=c++98

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