#include "includes/main.hpp"



int main(int c, char **v)
{
	if(c != 2)
		return 1;


	std::string path(v[1]);

	GlobaConfig config;
	if (parseConfig(path, config) == false)
		exit(1);
	server serv(config.servers);
	// 	return 1;
	serv.polling();
}