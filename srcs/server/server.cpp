#include "../../includes/server.hpp"
#include "../../includes/HttpHandler.hpp"

server::server(){}

server::~server()
{
	std::cout << "Destroctor: closing socketFds" << std::endl;
	for(size_t i = 0; i < socketFds.size(); i++){
		close(socketFds[i].fd);
	}
}

int server::listen_socket(std::string& ip, std::string& port)
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
	listenFds.push_back(sockfd);
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
	for(size_t i = 0; i < listenFds.size(); i++){
		if(fd == listenFds[i])
			return true;
	}
	return false;
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
		
		socketFds.push_back(create_pollfd(clientFd, POLLIN));

		inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
		unsigned short client_port = ntohs(client_addr.sin_port);

		std::cout << "New connection from: " << client_ip << ":" << client_port << std::endl;
		std::cout << "client nbr: " << socketFds.size() << " FD : " << clientFd << std::endl;
		std::memset(&client_addr, 0, client_len);
	}
}



int ft_recv(struct pollfd& pollfd, HttpHandler& http)
{
	char buf[1024];
	int read;

	read = recv(pollfd.fd, buf, sizeof(buf), 0);
	if(!read)
	{
		return 0;
	}
	http.appendData(buf, read);
	if(http.isComplete())
	{	
		pollfd.events = POLLOUT;
	}
	std::cout << "the receved bufer : " << buf << std::endl;
	return 1;
}

int ft_send(struct pollfd& pollfd, HttpHandler& http)
{
	// char d[100] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 29\r\n\r\n<h1>Hello from Server!  </h1>";
	
	std::vector<char> response = http.getResponse();
	// std::cout << "==================== response ==========================" <<std::endl;
	// std::cout.write(response.data(), response.size())<< std::endl;
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

int server::polling(std::string& path)
{
	////Users/haouky/Desktop/WebServ/srcs/http/config.conf
	GlobaConfig config = parseConfig(path);
	HttpHandler http(config.servers[0]);

	while (true)
	{
		std::cout << "=======================================start polling================================" << std::endl;
		int n = poll(&socketFds[0], socketFds.size(), -1);
		if(n < 0)
			std::cerr << "Poll : " << strerror(errno) << std::endl;

		for(size_t i = 0; i < socketFds.size() && n > 0 ; i++){
			std::cout << "nbr of client left to handle : " << n << std::endl;
			if(socketFds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
			{
				std::cout << "closing the sockefd : " << socketFds[i].fd << std::endl;
				close(socketFds[i].fd);
				socketFds.erase(socketFds.begin() + i);
				i--;
			}
			else if(socketFds[i].revents & POLLIN){
				
				if(is_listener(socketFds[i].fd))
					acceptClient(socketFds[i].fd);
				else
				{
					if(!ft_recv(socketFds[i], http))
					{
						std::cout << "closing the sockefd : " << socketFds[i].fd << std::endl;
						close(socketFds[i].fd);
						socketFds.erase(socketFds.begin() + i);
						i--;
					}
				}
				n--;
			}
			else if(socketFds[i].revents & POLLOUT)
			{
				ft_send(socketFds[i], http);
				n--;
			}

		}
	}
		
}

 