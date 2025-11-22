#ifndef CLIENT_HPP
#define CLIENT_HPP

#include"Headers.hpp"

#define BUFFER 4096
#define CGI_TIMEOUT 10
#define CLIENT_TIMEOUT 60
	
class client{
private:
	client();

	HttpHandler clientHandler;
	std::vector<ServerConfig> myServers;

	std::vector<char> requestData;
	std::vector<char> response;

	bool hostSeted;	
	bool responseComplete;
	size_t  totalsend;
	size_t  totalrecv;
	int fd;
	int cgiFd;

	std::time_t cgiStartTime;
	std::time_t clientLastActivity;
	
public:

	client(std::vector<ServerConfig>& myServers, int fd);
	client(const client& other);
	client& operator=(const client& other);
	~client();

	int getFd();

	int getCgiFd();
	void resetCgiFd();

	int cgiRun();
	void cgiCleaner();
	
	void startTimer();
	bool cgiTimeOut();
	
	void setupLastActivity();
	bool clientTimeOut();
	
	void setErrorResponse(HttpStatusCode statuCode);
	
	bool appendFirstRequest(const char* buf, ssize_t read);
	bool isHostSeted();
	void setHost(std::string &host);

	ssize_t ft_recv(short& event);
	ssize_t ft_send(short& event);
	ssize_t sending(short& event);
}; 


#endif 


