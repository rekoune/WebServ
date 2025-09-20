#include "../../includes/main_server.hpp"

int main(int c, char **v)
{
	if(c < 3)
		return 1;
	server serv;

	std::string ip(v[1]);
	std::string port(v[2]);
	if(serv.listen_socket(ip, port) == -1)
		return 1;

	serv.polling();
}