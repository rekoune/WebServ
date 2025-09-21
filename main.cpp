#include "includes/main.hpp"


int main(int c, char **v)
{
	if(c < 4)
		return 1;
	server serv;

	std::string ip(v[1]);
	std::string port(v[2]);
	std::string path(v[3]);
	if(serv.listen_socket(ip, port) == -1)
		return 1;
	
	serv.polling(path);
}