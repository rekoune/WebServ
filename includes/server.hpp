#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <vector>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h> 
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>



//wait connections 
// #define BACKLOG 10
class server {
private:
	std::vector<struct pollfd> socketFds;
	std::vector<int> listenFds;

	bool is_listener(int fd);
	unsigned int listenersNbr;
public:
	server();
	// server(std::string& IpPort);
	// server(const server& other);
	// server& operator=(const server& other);
	~server();

	int listen_socket(std::string& ip, std::string& port);
	struct pollfd create_pollfd(int fd, short events);
	int polling(std::string& path);

	void acceptClient(int listenFd);





};

#endif
