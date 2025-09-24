#include "../../includes/main_server.hpp"


server::server() : listenersNbr(0){}


server::server(std::vector<ServerConfig>&	servers): listenersNbr(0) {

	printingserver(servers[0]);

	std::map<std::string, int> iportToSocket;
	std::map<std::string, int>::iterator socketIt;

	for(size_t i = 0; i < servers.size(); i++)
	{
		std::map<std::string, std::vector<std::string> >& host_port = servers[i].host_port;
		std::map<std::string, std::vector<std::string> >::iterator it = host_port.begin();

		while(it != host_port.end()){
			for(size_t i = 0; i < it->second.size(); i++){
				std::string socket = it->first + ":" + it->second[i];
				std::cout<< "socket ip:port : |"<< socket << "|" << std::endl;
				socketIt = iportToSocket.find(socket);
				if(socketIt != iportToSocket.end())
				{
					listenToHosts[socketIt->second].push_back(servers[i]);
					
				}
				else
				{
					int socketfd;
					if((socketfd = listen_socket(it->first, it->second[i])) != -1){
						listenToHosts[socketfd].push_back(servers[i]);
						std::cout << "pushing the server--" << std::endl;
						printingserver(listenToHosts[socketfd].at(0));
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
	std::cout << "Destroctor: closing socketFds" << std::endl;
	for(size_t i = 0; i < socketFds.size(); i++){
		std::cout << "closing fd : " <<  socketFds[i].fd << std::endl;
		close(socketFds[i].fd);
	}
}

int server::listen_socket(const std::string& ip, const std::string& port)
{
	struct addrinfo *res, info;

	std::cout << "creat a listen socket in : |" << ip << ":" << port << std::endl;

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
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::cerr << "setsockopt(SO_REUSEADDR) error : " << strerror(errno) << std::endl;
		//return ?
	}
	
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
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	//ERROR ?

	listenersNbr++;
	// listenFds.push_back(sockfd);
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
	
	// for(size_t i = 0; i < listenFds.size(); i++){
	// 	if(fd == listenFds[i])
	// 		return true;
	// }
}


void server::acceptClient(int listenFd)
{
	struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char client_ip[INET_ADDRSTRLEN]; //shoue be rmoved /`
	
	int clientFd ;
	
	while((clientFd = accept(listenFd,(struct sockaddr*)&client_addr, &client_len)))
	{
		if(clientFd < 0)
			break;

		int flags = fcntl(clientFd, F_GETFL, 0);
		fcntl(clientFd, F_SETFL, flags | O_NONBLOCK);
		//ERROR ?
		
		clients.push_back(client(listenToHosts[listenFd], clientFd));
		socketFds.push_back(create_pollfd(clientFd, POLLIN));

		inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
		unsigned short client_port = ntohs(client_addr.sin_port);

		std::cout << "New connection from: " << client_ip << ":" << client_port << std::endl;
		std::cout << "client nbr: " << socketFds.size() << " FD : " << clientFd << std::endl;
		std::memset(&client_addr, 0, client_len);
	}
}



int server::ft_recv(struct pollfd& pollfd, int i)
{
	char buf[BUFFER];
	int read;

	read = recv(pollfd.fd, buf, sizeof(buf), 0);
	// buf[read] = 0;
	if(pollfd.fd != clients[i].getFd())
		  std::cout << "\033[31mthis is not the correct client \ni : " << i << "\033[0m" << std::endl;
	else
		std::cout << "\033[32mthis is the correct client \n i: " << i << "\033[0m" << std::endl;

	std::cout<< "buf: " << buf<< std::endl;
	if(!clients[i].isHostSeted())
	{
		std::cout << "seting host...." << std::endl;
		if(clients[i].appendFirstRequest(buf, read))
			pollfd.events = POLLOUT;
		return read;
	}
	if(read){
		clients[i].appendData(buf, read);
		if(clients[i].isComplete())
			pollfd.events = POLLOUT;
	}
	// std::cout << "the receved bufer : " << buf << std::endl;
	return 1;
}

int server::ft_send(struct pollfd& pollfd, int i)
{

	std::vector<char> response = clients[i].getResponse();
	std::cout << "==================== response ==========================" <<std::endl;
	std::cout.write(response.data(), response.size())<< std::endl;
	int n  = send(pollfd.fd, &response[0], response.size(), 0);
	
	if(n > 0)
	{
		std::cout << "succufly send!!!!!" << std::endl;
		pollfd.events = POLLIN;
	}
	else
		std::cout << "send failed" << std::endl;
	return 1;
}

int Working_flage = 1;

void handleSigint(int sig) {
    (void)sig; 
    Working_flage = 0;
    std::cout << "\nshutting down..." << std::endl;
}

int server::polling()
{

	signal(SIGINT, handleSigint); 

	while (Working_flage)
	{
		std::cout << "=======================================start polling================================" << std::endl;
		int NbrOfActiveSockets = poll(&socketFds[0], socketFds.size(), -1);
		if(NbrOfActiveSockets < 0)
			std::cerr << "Poll : " << strerror(errno) << std::endl;

		for(size_t i = 0; i < socketFds.size() && NbrOfActiveSockets > 0 ; i++){
			std::cout << "nbr of client left to handle : " << NbrOfActiveSockets << std::endl;
			if(socketFds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
			{
				std::cout << "closing the sockefd : " << socketFds[i].fd << std::endl;
				close(socketFds[i].fd);
				socketFds.erase(socketFds.begin() + i);
				i--;
			}
			else if((socketFds[i].revents & POLLIN)  && Working_flage){
				
				if(is_listener(socketFds[i].fd))
					acceptClient(socketFds[i].fd);
				else
				{
					if(!ft_recv(socketFds[i], i - listenersNbr))
					{
						std::cout << "closing the sockefd : " << socketFds[i].fd << std::endl;
						close(socketFds[i].fd);
						socketFds.erase(socketFds.begin() + i);
						i--;
					}
				}
				NbrOfActiveSockets--;
			}
			else if((socketFds[i].revents & POLLOUT)  && Working_flage)
			{
				ft_send(socketFds[i], i - listenersNbr);
				NbrOfActiveSockets--;
			}

		}
	}
	return 1;
}

 