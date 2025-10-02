#include "includes/main.hpp"



int main(int c, char **v)
{
	if(c != 2)
		return 1;
	if(c == 1){
		std::cout << "the default config ??????" << std::endl;
		return 1;
	}

	std::string path(v[1]);

	GlobaConfig config;
	if (parseConfig(path, config) == false)
		exit(1);
	std::cout << "\033[1;36mThe server starts\033[0m" << std::endl; // Bright cyan
	server serv(config.servers);
	// 	return 1;
	serv.polling();
}