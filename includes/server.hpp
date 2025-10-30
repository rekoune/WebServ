#ifndef SERVER_HPP
# define SERVER_HPP


#include"Headers.hpp"

#include "client.hpp"




class server {
private:
	server();
	
	std::vector<struct pollfd> socketFds;
	std::map<int, client> clients;
	std::map<int, client*> cgi;

	std::map<int, std::vector<ServerConfig> > listenToHosts; 

	bool is_listener(int fd);
	bool is_cgi(int fd);
public:
	server(std::vector<ServerConfig>&	servers);
	server(const server& other);
	server& operator=(const server& other);
	~server();

	int listen_socket(const std::string& ip, const std::string& port);
	struct pollfd create_pollfd(int fd, short events);
	int polling();

	client& getClient(int& fd);
	void acceptClient(int listenFd);
	void rmClient(size_t &i);

};




#endif

