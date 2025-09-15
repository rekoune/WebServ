SRC 	=  $(wildcard srcs/config/*.cpp srcs/server/*.cpp srcs/http/*.cpp srcs/utils/*.cpp srcs/main.cpp)

OBJ 	= $(SRC:.cpp=.o) 

FLAGS 	= -Wall -Wextra -Werror -std=c++98

NAME = webserv

all : $(NAME)

$(NAME) : $(OBJ)
	c++ $(FLAGS) $(OBJ) -o $(NAME)


%.o:%.cpp 
	c++ $(FLAGS) -c $< -o $@

clean :
	$(RM) $(OBJ)

fclean : clean
	$(RM) $(NAME)

re : fclean all 

.PHONY: clean
