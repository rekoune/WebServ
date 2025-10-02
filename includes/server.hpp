#ifndef SERVER_HPP
# define SERVER_HPP


#include"Headers.hpp"

#include "client.hpp"




class server {
private:
	server();
	
	std::vector<struct pollfd> socketFds;
	std::vector<client> clients;

	std::map<int, std::vector<ServerConfig> > listenToHosts; 

	bool is_listener(int fd);
	unsigned int listenersNbr;

	
public:
	server(std::vector<ServerConfig>&	servers);
	server(const server& other);
	server& operator=(const server& other);
	~server();

	int listen_socket(const std::string& ip, const std::string& port);
	struct pollfd create_pollfd(int fd, short events);
	int polling();

	void acceptClient(int listenFd);

};


#endif

