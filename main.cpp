#include "includes/main.hpp"


bool workFlage = true;

void handleSigint(int sig) {
    (void)sig; 
    workFlage = false;
}


int main(int c, char **v)
{
	signal(SIGINT, handleSigint);
	if(c > 2){	
		std::cerr << "Too many files enter one config file or none to use the default one !!" << std::endl; 
		return 1;
	}
	std::string path("config/default.conf");
	if(c == 2)
		path = v[1];
	
	GlobaConfig config;
	if (!parseConfig(path, config))
		return 1;
	server serv(config.servers);
	serv.serverCore();
}