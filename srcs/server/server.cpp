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

void server::rmClient(size_t &i){
	std::cout << "\033[31mclosing the sockefd : " << socketFds[i].fd <<"\033[0m" << std::endl;
	close(socketFds[i].fd);
	clients.erase(socketFds[i].fd);
	socketFds.erase(socketFds.begin() + i);
	i--;
}

client& server::getClient(int& fd){
	return clients.find(fd)->second;
}


int server::polling()
{
	if(listenToHosts.empty()){
		std::cerr << "no listen Fds" << std::endl;
		return 0;
	}

	signal(SIGPIPE, SIG_IGN);
	
	while (workFlage)
	{
		// sleep(1);
		std::cout << "=======================================start polling================================" << std::endl;
		int NbrOfActiveSockets = poll(&socketFds[0], socketFds.size(), -1);
		if(NbrOfActiveSockets < 0)
			std::cerr << "Poll : " << strerror(errno) << std::endl;

		for(size_t i = 0; i < socketFds.size() && NbrOfActiveSockets > 0 ; i++){

			std::cout << "nbr of client left to handle : " << NbrOfActiveSockets << " this is fd : "<< socketFds[i].fd<< std::endl;
			client* currentClient = &getClient(socketFds[i].fd);

			// if(socketFds[i].revents & (POLLHUP | POLLERR | POLLNVAL)){
			// 	if(is_cgi(socketFds[i].fd))
			// 	{
			// 		std::cout << "cgi try to exit..." << std::endl;
			// 		continue;
			// 	}
			// 	rmClient(i);
			// 	NbrOfActiveSockets--;
			// }
			if(workFlage && (socketFds[i].revents & POLLIN)){
				std::cout << "POLLIN FD: " << socketFds[i].fd << std::endl;
				if(is_listener(socketFds[i].fd))
						acceptClient(socketFds[i].fd);
				else if(is_cgi(socketFds[i].fd)){
					int cgiStatus = cgi[socketFds[i].fd]->cgiRun();
					if(cgiStatus == -1){
						cgi.erase(socketFds[i].fd);
						socketFds.erase(socketFds.begin() + i);
						i--;
						NbrOfActiveSockets--;
					}
				}
				else
				{
					if(!currentClient->ft_recv(socketFds[i].events)){
						rmClient(i); 
						continue;
					}
					int cgiFd = currentClient->getCgiFd(); //here abort
					if(cgiFd != -1){
						cgi.insert(std::make_pair(cgiFd, currentClient));
						std::cout << "cgi fd is : " << cgiFd << std::endl;
						int flags = fcntl(cgiFd, F_GETFL, 0);
						if(flags == -1 || fcntl(cgiFd, F_SETFL, flags | O_NONBLOCK) == -1){
							std::cerr << "fcntl error: " << strerror(errno) << std::endl;
							close(cgiFd);
							rmClient(i);
							continue;
						}
						socketFds.push_back(create_pollfd(cgiFd, POLLIN));
						cgi.insert(std::make_pair(cgiFd, currentClient));
					}
				}
				NbrOfActiveSockets--;
			}
			else if(workFlage && (socketFds[i].revents & POLLOUT))
			{
				std::cout << "POLLOUT FD: " << socketFds[i].fd  << std::endl;
				NbrOfActiveSockets--;
				if(currentClient->getCgiFd() != -1){
					std::cout << "continue " << std::endl;
					continue;
				}
				if(!currentClient->ft_send(socketFds[i].events))
						rmClient(i);
			}
			else if(socketFds[i].revents & (POLLHUP | POLLERR | POLLNVAL)){
				if(is_cgi(socketFds[i].fd))
				{
					std::cout << "cgi try to exit..." << std::endl;
					continue;
				}
				rmClient(i);
				NbrOfActiveSockets--;
			}

		}
	}
	return 1;
}

 