#include "includes/main.hpp"



int main(int c, char **v)
{
	if(c > 2){
		std::cout << "Entry one config file or non to use the default one" << std::endl; 
		return 1;
	}

	std::string path;
	if(c != 1)
		path = v[1];

	GlobaConfig config;
	if (!parseConfig(path, config))
		return 1;
	std::cout << "\033[1;35mThe server starts\033[0m" << std::endl; // Bright magenta (mauve)
	server serv(config.servers);
	serv.polling();
}