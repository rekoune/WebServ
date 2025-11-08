#include "../../includes/main_server.hpp"



server::server(const server& other) :
									socketFds(other.socketFds), clients(other.clients),
									listenToHosts(other.listenToHosts) {}

server& server::operator=(const server& other){
	if(this != &other){
		socketFds = other.socketFds;
		clients = other.clients;
		listenToHosts = other.listenToHosts;
	}
	return *this;
}

server::server(std::vector<ServerConfig>&	servers)  {
	std::map<std::string, int> iportToSocket; //ip:port -> socket;
	std::map<std::string, int>::iterator socketIt; // |^| there iterotor

	for(size_t i = 0; i < servers.size(); i++)
	{
		std::map<std::string, std::vector<std::string> >& host_port = servers[i].host_port; //host(ip) -> port
		std::map<std::string, std::vector<std::string> >::iterator it = host_port.begin(); // |^| there iterotor

		while(it != host_port.end()){
			for(size_t j = 0; j < it->second.size(); j++){
				std::string socket = it->first + ":" + it->second[j];
				socketIt = iportToSocket.find(socket);
				if(socketIt != iportToSocket.end()){
					listenToHosts[socketIt->second].push_back(servers[i]); //listenToHost => socketfd -> serversconfig
					//it's a listening socket to it's possible servers;
				}
				else
				{
					int socketfd;
					if((socketfd = listen_socket(it->first, it->second[j])) != -1){
						listenToHosts[socketfd].push_back(servers[i]);
						iportToSocket[socket] = socketfd;
					}
				}
			}
			it++;
		}
	}
}

server::~server()
{
	std::cout << "\033[31mDestroctor: closing socketFds\033[0m" << std::endl;
	for(size_t i = 0; i < socketFds.size(); i++){
		std::cout << "closing fd : " <<  socketFds[i].fd << std::endl;;
		close(socketFds[i].fd);
	}
}

int server::listen_socket(const std::string& ip, const std::string& port)
{
	struct addrinfo *res, info;

	std::cout << "creat a listen socket in : " << ip << ":" << port << std::endl;

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


bool server::is_listener(int fd)
{
	std::map<int, std::vector<ServerConfig> >::iterator it = listenToHosts.begin();
	while(it != listenToHosts.end())
	{
		if(fd == it->first)
			return true;
		it++;
	}
	return false;
	
}


void server::acceptClient(int listenFd)
{
	struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
	
	int clientFd;
	
	while((clientFd = accept(listenFd, (struct sockaddr*)&client_addr, &client_len)) >= 0)
	{

		int flags = fcntl(clientFd, F_GETFL, 0);
		if(flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1){
			std::cerr << "fcntl error: " << strerror(errno) << std::endl;
			close(clientFd);
			continue;
		}

		clients.insert(std::make_pair(clientFd, client(listenToHosts[listenFd], clientFd)));
		socketFds.push_back(create_pollfd(clientFd, POLLIN));

		std::cout << "\033[32mNew connection Fd : "<< clientFd <<"\033[0m" << std::endl;
		std::memset(&client_addr, 0, client_len);
	}
}

bool server::is_cgi(int fd){
	std::map<int, client* >::iterator it = cgi.begin();
	while(it != cgi.end())
	{
		if(fd == it->first)
			return true;
		it++;
	}
	return false;
}

void server::rmClient(size_t &fdIndex)
{
	std::cout << "\033[31mclosing the sockefd : " << socketFds[fdIndex].fd <<"\033[0m" << std::endl;
	int fd = socketFds[fdIndex].fd;
	int cgiFd = -1;
	std::map<int, client>::iterator it = clients.find(fd);
	if (it != clients.end())
		cgiFd = it->second.getCgiFd();
		if(cgiFd != -1){
		cgi.erase(cgiFd);
		for(size_t i = 0; i < socketFds.size(); i++){
			if(socketFds[i].fd == cgiFd)
			socketFds.erase(socketFds.begin() + i);
			if(i < fdIndex)
			fdIndex--;
			break;
		}
	}
	close(fd);
	clients.erase(fd);
	socketFds.erase(socketFds.begin() + fdIndex);
	fdIndex--;
}

client& server::getClient(int& fd){
	return clients.find(fd)->second;
}

void server::cgiSetup(size_t& fdIndex, int cgiFd){
	cgi.insert(std::make_pair(cgiFd, currentClient));
	std::cout << "cgi fd is : " << cgiFd << std::endl;
	int flags = fcntl(cgiFd, F_GETFL, 0);
	if(flags == -1 || fcntl(cgiFd, F_SETFL, flags | O_NONBLOCK) == -1){
		std::cerr << "fcntl error: " << strerror(errno) << std::endl;
		close(cgiFd);
		rmClient(fdIndex);
		return ;
	}
	socketFds.push_back(create_pollfd(cgiFd, POLLIN));
	cgi.insert(std::make_pair(cgiFd, currentClient));
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
			rmCgi(fdIndex, true);
	}
	else
	{
		if(!currentClient->ft_recv(pfd.events)){
			rmClient(fdIndex); 
			return ;
		}
		int cgiFd = currentClient->getCgiFd(); 
		if(cgiFd != -1){
			cgiSetup(fdIndex, cgiFd);
			currentClient->startTimer();
		}
	}
}

void server::pollout(size_t& fdIndex)
{
	struct pollfd& pfd = socketFds[fdIndex];
	std::cout << "POLLOUT FD: " << pfd.fd  << std::endl;
	if(currentClient->getCgiFd() != -1){
		if(currentClient->checkTimeOut()){
			for(size_t i = 0; i < socketFds.size(); i++){
				if(currentClient->getCgiFd() == socketFds[i].fd){
					std::cout << "\033[33mTimeout occurred, setting error response.\033[0m" << std::endl;
					rmCgi(i, false);
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

void server::rmCgi(size_t& fdIndex, bool workDone){
	struct pollfd& pfd = socketFds[fdIndex];
	cgi[pfd.fd]->resetCgiFd();
	if(!workDone)
		cgi[pfd.fd]->setErrorResponse();
	cgi.erase(socketFds[fdIndex].fd);
	socketFds.erase(socketFds.begin() + fdIndex);
	fdIndex--;
}


int server::serverCore()
{
	if(listenToHosts.empty()){
		std::cerr << "Error: no listen Fds" << std::endl;
		return 1;
	}
	signal(SIGPIPE, SIG_IGN);	
	while (workFlage)
	{
		// sleep(1);
		std::cout << "=======================================start the poll================================" << std::endl;
		int NbrOfActiveSockets = poll(&socketFds[0], socketFds.size(), -1);
		if(NbrOfActiveSockets < 0)
			std::cerr << "Poll : " << strerror(errno) << std::endl;
		for(size_t i = 0; i < socketFds.size() && NbrOfActiveSockets > 0 ; i++){
			std::cout << "nbr of client left to handle : " << NbrOfActiveSockets << " this is fd : "<< socketFds[i].fd<< std::endl;
			currentClient = &getClient(socketFds[i].fd);
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
				if(is_cgi(socketFds[i].fd)){
					rmCgi(i, false);
					continue;
				}
				rmClient(i);
			}
		}
	}
	return 0;
}

 