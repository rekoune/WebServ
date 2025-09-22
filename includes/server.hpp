#ifndef SERVER_HPP
# define SERVER_HPP


#include"Headers.hpp"

#include "client.hpp"

#define BUFFER 4096

// struct hosts{
// 	std::string ipPort;
// 	std::string socket;
// 	std::vector<std::string> hosts;
// };

class server {
private:
	std::vector<struct pollfd> socketFds;

	// std::vector<int> listenFds;

	std::vector<client> clients;

	std::map<int, std::vector<ServerConfig> > listenToHosts;

	

	bool is_listener(int fd);
	unsigned int listenersNbr;
public:
	server();
	server(std::vector<ServerConfig>&	servers);
	// server(const server& other);
	// server& operator=(const server& other);
	~server();

	int listen_socket(const std::string& ip, const std::string& port);
	struct pollfd create_pollfd(int fd, short events);
	int polling();

	void acceptClient(int listenFd);

	int ft_send(struct pollfd& pollfd, int i);
	int ft_recv(struct pollfd& pollfd, int i);

};

#endif


// struct ServerConfig
// {

// std::string servername;
// 	std::map<std::string, std::vector<std::string> >	host_port;
//     std::string					        		root;
//     size_t						        		client_max_body_size;
//     std::map<int, std::string>	        		error_pages;
// 	std::vector<LocationConfig>	        		locations;
//     std::map<std::string, std::string>  		cgi_extension;
//     ServerConfig() 
//         :  root("/"),  client_max_body_size(1024 * 1024)  
// 		{}
	
// };