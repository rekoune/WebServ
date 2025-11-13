#include "includes/main.hpp"


bool workFlage = true;

void handleSigint(int sig) {
    (void)sig; 
    workFlage = false;
    std::cout << "\nshutting down..." << std::endl;
}


int main(int c, char **v)
{
	signal(SIGINT, handleSigint);


	if(c > 2){
		std::cout << "Entry one config file or non to use the default one" << std::endl; 
		return 1;
	}

	std::string path;
	if(c == 2)
		path = v[1];

	GlobaConfig config;
	if (!parseConfig(path, config))
		return 1;
	std::cout << "\033[1;35mThe server starts\033[0m" << std::endl; 
	server serv(config.servers);
	serv.serverCore();
}