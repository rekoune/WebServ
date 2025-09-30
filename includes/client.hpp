#ifndef CLIENT_HPP
#define CLIENT_HPP

#include"Headers.hpp"

#define BUFFER 4096

class client{
private:
	client();

	HttpHandler clientHandler;
	std::vector<ServerConfig> myServers;

	std::vector<char> requestData;
	std::vector<char> response;

	bool hostSeted;	
	bool responseComplete; 
	size_t  totalsend ;
	int fd;

public:

	client(std::vector<ServerConfig>& myServers, int fd);
	client(const client& other);
	client& operator=(const client& other);
	~client();

	int getFd();

	bool appendFirstRequest(const char* buf, ssize_t read);
	bool isHostSeted();
	void setHost(std::string &host);

	ssize_t ft_recv(short& event);
	ssize_t ft_send(short& event);
	ssize_t sending(short& event);
}; 

// void printingserver(const ServerConfig& servers);

#endif 


