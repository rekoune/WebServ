#ifndef CLIENT_HPP
#define CLIENT_HPP

#include"Headers.hpp"

class client{
private:
	client();
		HttpHandler socketHttp;

		std::vector<ServerConfig>& myServers;
		std::string requestData;
		std::string hostName;
		bool hostSeted;
	public:

		client(std::vector<ServerConfig>& myServers);
		// client(const client& other);
		// client& operator=(const client& other);
		// ~client();

		void appendFirstRequest(const char* buf, int read);
		bool isHostSeted();
		void setHost(std::string &host);

		void appendData(const char* data, size_t size);
		bool isComplete();
		std::vector<char> getResponse();
};


#endif 


