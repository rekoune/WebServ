#ifndef CLIENT_HPP
#define CLIENT_HPP

#include"Headers.hpp"

class client{
private:
	client();
		HttpHandler socketHttp;
		std::vector<ServerConfig>& myServers;

		std::vector<char> requestData;
		bool hostSeted ;	
		int fd;

	public:

		client(std::vector<ServerConfig>& myServers, int fd);
		client(const client& other);
		// client& operator=(const client& other);
		// ~client();
		int getFd();
		bool appendFirstRequest(const char* buf, int read);
		bool isHostSeted();
		void setHost(std::string &host);

		void appendData(const char* data, size_t size);
		bool isComplete();
		std::vector<char> getResponse();
};
void printingserver(const ServerConfig& servers);

#endif 


