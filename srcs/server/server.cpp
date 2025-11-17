#include "../../includes/main_server.hpp"



server::server(const server& other) :
									socketFds(other.socketFds), clients(other.clients),
									socketToServers(other.socketToServers) {}

server& server::operator=(const server& other){
	if(this != &other){
		socketFds = other.socketFds;
		clients = other.clients;
		socketToServers = other.socketToServers;
	}
	return *this;
}

server::server(std::vector<ServerConfig>&	servers)  {
	std::map<std::string, int> iportToSocket; //ip:port -> socket;p
	std::map<std::string, int>::iterator socketIt; // |^| there iterotor

	for(size_t i = 0; i < servers.size(); i++)
	{
		std::map<std::string, std::vector<std::string> >& ipToPorts = servers[i].host_port; //host(ip) -> port
		std::map<std::string, std::vector<std::string> >::iterator ipPortIt= ipToPorts.begin(); // |^| there iterotor

		while(ipPortIt != ipToPorts.end()){
			for(size_t j = 0; j < ipPortIt->second.size(); j++){
				std::string ipPortStr = ipPortIt->first + ":" + ipPortIt->second[j];
				socketIt = iportToSocket.find(ipPortStr);
				if(socketIt != iportToSocket.end()){
					socketToServers[socketIt->second].push_back(servers[i]); //listenToHost => socketfd -> serversconfig
					//it's a listening socket to it's possible servers;
				}
				else
				{
					int socketfd;
					if((socketfd = listen_socket(ipPortIt->first, ipPortIt->second[j])) != -1){
						socketToServers[socketfd].push_back(servers[i]);
						iportToSocket[ipPortStr] = socketfd;
					}
				}
			}
			ipPortIt++;
		}
	}
	std::cout << "\033[1;34mServer configuration setup completed.\033[0m" << std::endl;
}

server::~server()
{
	std::cout << "\033[31mDestructor: shutting down the server\033[0m" << std::endl;
	for(size_t i = 0; i < socketFds.size(); i++){
		int fd = socketFds[i].fd;
		if(is_cgi(fd))
			cgi[fd]->cgiCleaner();
		else
			close(fd);
	}
}

int server::listen_socket(const std::string& ip, const std::string& port)
{
	struct addrinfo *res, info;

	
	std::memset(&info, 0, sizeof(info));
	info.ai_family = AF_INET;       
	info.ai_socktype = SOCK_STREAM; 
	info.ai_flags = AI_PASSIVE;     // That is For binding
	
	int status = getaddrinfo(ip.c_str(), port.c_str(), &info, &res);
	if(status){
		std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
		return -1;
	}
	
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd == -1) {
		std::cerr << "socket creation error: " << strerror(errno) << std::endl;
		freeaddrinfo(res); 
		return -1;
	}	
	
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	std::cerr << "setsockopt(SO_REUSEADDR) error : " << strerror(errno) << std::endl;
	
	if(bind(sockfd, res->ai_addr, res->ai_addrlen) == -1){
		std::cerr << "bind error on " << ip << ":" << port << ": " << strerror(errno) << std::endl;
		close(sockfd);    
		freeaddrinfo(res);
		return -1;
	}
	freeaddrinfo(res);
	
	if (listen(sockfd, SOMAXCONN) == -1) {
		std::cerr << "listen error: " << strerror(errno) << std::endl;
		close(sockfd);
		return -1;
	}
	
	int flags = fcntl(sockfd, F_GETFL, 0);
	if(flags == -1 || fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1){
		std::cerr << "fcntl error: " << strerror(errno) << std::endl;
		close(sockfd);
		return -1;
	}
	std::cout << "Server is listening on " << ip << " :" << port << std::endl;
	socketFds.push_back(create_pollfd(sockfd, POLLIN));	
	return sockfd;
}


struct pollfd server::create_pollfd(int fd, short events) 
{
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = events;
    pfd.revents = 0;
    return pfd; 
}

void server::acceptClient(int listenFd)
{
	struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
	
	int clientFd;
	
	while((clientFd = accept(listenFd, (struct sockaddr*)&client_addr, &client_len)) >= 0){
		int flags = fcntl(clientFd, F_GETFL, 0);
		if(flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1){
			std::cerr << "fcntl error: " << strerror(errno) << std::endl;
			close(clientFd);
			continue;
		}
		
		struct linger sl;
        sl.l_onoff = 1;   
        sl.l_linger = 0;  
        if (setsockopt(clientFd, SOL_SOCKET, SO_LINGER, &sl, sizeof(sl)) < 0) {
			std::cerr << "setsockopt(SO_LINGER) error: " << strerror(errno) << std::endl;
        }
		clients.insert(std::make_pair(clientFd, client(socketToServers[listenFd], clientFd)));
		socketFds.push_back(create_pollfd(clientFd, POLLIN));
		
		std::cout << "\033[32mNew connection Fd : "<< clientFd <<"\033[0m" << std::endl;
		std::memset(&client_addr, 0, client_len);
	}
}
bool server::is_listener(int fd)
{
	std::map<int, std::vector<ServerConfig> >::iterator it = socketToServers.begin();
	while(it != socketToServers.end())
	{
		if(fd == it->first)
			return true;
		it++;
	}
	return false;
	
}

bool server::is_cgi(int fd){
	if(cgi.empty())
		return false;
	std::map<int, client* >::iterator it = cgi.begin();
	while(it != cgi.end())
	{
		if(fd == it->first)
			return true;
		it++;
	}
	return false;
}


client* server::getClient(int& fd){
	std::map<int, client>::iterator it = clients.find(fd);
	if (it != clients.end())
	return &(it->second);
	return NULL;
}

void server::cgiSetup(int cgiFd){
	int flags = fcntl(cgiFd, F_GETFL, 0);
	if(flags == -1 || fcntl(cgiFd, F_SETFL, flags | O_NONBLOCK) == -1){
		std::cerr << "fcntl error: " << strerror(errno) << std::endl;
		close(cgiFd);
		currentClient->setErrorResponse(INTERNAL_SERVER_ERROR);
		currentClient->cgiCleaner();
		return ;
	}
	socketFds.push_back(create_pollfd(cgiFd, POLLIN));
	cgi.insert(std::make_pair(cgiFd, currentClient));
}


void server::rmClient(size_t &fdIndex)
{
	int fd = socketFds[fdIndex].fd;
	std::cout << "\033[31mclosing the socketFd : " << fd <<"\033[0m" << std::endl;
	int cgiFd = -1;
	cgiFd = currentClient->getCgiFd();
	if(cgiFd != -1){
		for(size_t i = 0; i < socketFds.size(); i++){
			if(socketFds[i].fd == cgiFd){
				rmCgi(i, true, OK);
				if(i < fdIndex)
					fdIndex--;
				break;
			}
		}
	}
	close(fd);
	clients.erase(fd);
	socketFds.erase(socketFds.begin() + fdIndex);
	fdIndex--;
}
void server::rmCgi(size_t& fdIndex, bool workDone, HttpStatusCode statuCode){
	struct pollfd& pfd = socketFds[fdIndex];
	cgi[pfd.fd]->resetCgiFd();
	if(!workDone)
		cgi[pfd.fd]->setErrorResponse(statuCode);
	cgi.erase(socketFds[fdIndex].fd);
	socketFds.erase(socketFds.begin() + fdIndex);
	fdIndex--;
}

void server::pollin(size_t &fdIndex)
{
	struct pollfd& pfd = socketFds[fdIndex];
	std::cout << "POLLIN FD: " << pfd.fd << std::endl;
	if(is_listener(pfd.fd))
		acceptClient(pfd.fd);
	else if(is_cgi(pfd.fd)){
		int cgiStatus = cgi[pfd.fd]->cgiRun();
		if(cgiStatus == -1)
			rmCgi(fdIndex, true, OK);
	}
	else if(currentClient)
	{
		currentClient->setupLastActivity();
		if(!currentClient->ft_recv(pfd.events)){
			rmClient(fdIndex); 
			return ;
		}
		int cgiFd = currentClient->getCgiFd(); 
		if(cgiFd != -1){
			cgiSetup(cgiFd);
			currentClient->startTimer();
		}
	}
}

void server::pollout(size_t& fdIndex)
{
	struct pollfd& pfd = socketFds[fdIndex];
	std::cout << "POLLOUT FD: " << pfd.fd  << std::endl;
	currentClient->setupLastActivity();
	if(currentClient->getCgiFd() != -1){
		if(currentClient->cgiTimeOut()){
			for(size_t i = 0; i < socketFds.size(); i++){
				if(currentClient->getCgiFd() == socketFds[i].fd){
					std::cout << "\033[33mCgi timeout occurred\033[0m" << std::endl;
					currentClient->cgiCleaner();
					rmCgi(i, false, REQUEST_TIME_OUT);
					if(i < fdIndex)
					fdIndex--; 
				}
			}
		}
		return;
	}
	if(!currentClient->ft_send(pfd.events))
			rmClient(fdIndex);
}

int server::serverCore()
{
	if(socketToServers.empty()){
		std::cerr << "Error: no listen Fds" << std::endl;
		return 1;
	}
	std::cout << "\033[1;35mThe server starts\033[0m" << std::endl; 
	signal(SIGPIPE, SIG_IGN);	
	while (workFlage)
	{
		// std::cout << "\033[1;36mWaiting for events... Polling active connections.\033[0m" << std::endl;
		int NbrOfActiveSockets = poll(&socketFds[0], socketFds.size(), 2000);
		if(NbrOfActiveSockets > 0)
			std::cout << "Number of active clients: " << NbrOfActiveSockets << std::endl;
		if(NbrOfActiveSockets < 0)
		std::cerr << "Poll : " << strerror(errno) << std::endl;
		for(size_t i = 0; i < socketFds.size()  ; i++){
			currentClient = getClient(socketFds[i].fd);
			if(workFlage && (socketFds[i].revents & POLLIN)){
				NbrOfActiveSockets--;
				pollin(i);
			}
			else if(workFlage && (socketFds[i].revents & POLLOUT)){
				NbrOfActiveSockets--;
				pollout(i);
			}
			else if(socketFds[i].revents & (POLLHUP | POLLERR | POLLNVAL)){
				NbrOfActiveSockets--;
				if(is_cgi(socketFds[i].fd))
					rmCgi(i, false, INTERNAL_SERVER_ERROR);
				else
					rmClient(i);
			}
			else if(currentClient && currentClient->clientTimeOut()){
				std::cout << "\033[31mTimeout occurred, closing client connection.\033[0m" << std::endl;
				rmClient(i);
			}
		}
	}
	return 0;
}

 