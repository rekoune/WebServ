STD = -std=c++98
CP = c++
FLAGS = -Wall -Wextra -Werror $(STD)
FILES = server.cpp tes.cpp
NAME = server
OBJ = $(FILES:.cpp=.o)

all: $(NAME)

$(NAME) : $(OBJ)
	$(CP) $(FLAGS)  $(OBJ) -o $(NAME)

%.o:%.cpp
	$(CP) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re : fclean all