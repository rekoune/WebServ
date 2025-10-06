#include "../../includes/client.hpp"

// void printingserver(const ServerConfig& servers){
// 	std::cout << "\033[34mroot: \033[0m" << servers.root << std::endl; // Blue text
// 	std::cout << "\033[32mlocation: \033[0m" << servers.locations.size() << std::endl; // Green text
// 	std::cout << "\033[35mserver_name: \033[0m" << servers.server_name[0] << std::endl; // Magenta text
// 	std::cout << "\033[36mbody max size: \033[0m" << servers.client_max_body_size << std::endl; // Cyan text
// }




client::~client() {}

client& client::operator=(const client& other){
	if(this != &other){
		clientHandler = other.clientHandler;
		myServers = other.myServers;
		requestData = other.requestData;					
		response = other.response;
		hostSeted = other.hostSeted;
		responseComplete = other.responseComplete;
		totalsend = other.totalsend;
		totalrecv = other.totalrecv;
		fd = other.fd;
	}
	return *this;
}


client::client(std::vector<ServerConfig>& myservers, int fd) : myServers(myservers) ,hostSeted(false),
		responseComplete(true) ,totalsend(0),totalrecv(0) ,fd(fd) 
{
	if(myServers.size() == 1){
		clientHandler.setServer(myservers[0]);
		hostSeted = true;
	}
}

client::client(const client& other): 
		clientHandler(other.clientHandler),myServers(other.myServers) ,requestData(other.requestData),
		response(other.response) ,hostSeted(other.hostSeted),responseComplete(other.responseComplete) ,
		totalsend(other.totalsend),totalrecv(other.totalrecv) , fd(other.fd) {}


ssize_t client::ft_recv(short& event){
	char buf[BUFFER];
	ssize_t read = recv(fd, buf, sizeof(buf), 0);
	if(read == -1){
		std::cerr << "Error receiving data: " << strerror(errno) << std::endl; 
		if(errno == EAGAIN || errno == EWOULDBLOCK)
			return 1;
		return 0;
	}
	else if(!isHostSeted()){
		std::cout << "seting host...." << std::endl;
		if(appendFirstRequest(buf, read)){
			totalrecv = 0;
			event = POLLOUT;
		}
	}
	else if(read){
		std::cout << "receving...." << std::endl;
		clientHandler.appendData(buf, read);
		if(clientHandler.isComplete()){
			std::cout << "all has been receved" << std::endl;
			totalrecv = 0;
 			event = POLLOUT;
		}
	}
	totalrecv += read;
	std::cout << "total recv is " << totalrecv << std::endl;

	return read;
}

ssize_t client::sending(short& event){
	ssize_t nsend;

	while (totalsend < response.size()){
		nsend = send(fd, &response[0] + totalsend, response.size() - totalsend, 0);
		if(nsend == -1){
			std::cerr << "send error: " << strerror(errno) << std::endl;
			if (errno == EAGAIN || errno == EWOULDBLOCK)
                return 1;
			return 0;
		}
		totalsend += nsend;
	}

	std::cout << "total data from Post = " << totalsend << std::endl;
	if(totalsend == response.size()){
		responseComplete = true;
		event = POLLIN;
		totalsend = 0;
		std::cout << "succufly send everyting !!!" << std::endl;
	}
	return nsend;
}


ssize_t client::ft_send(short& event){
	// ssize_t  nsend ;

	if(responseComplete){
		response = clientHandler.getResponse();
		responseComplete = false;
	}
	std::cout << "sneding... " << std::endl;
	return sending(event);
}

bool client::isHostSeted(){
	return hostSeted;
}

int client::getFd(){
	return fd;
}

void client::setHost(std::string &host){

	for(size_t i = 0; i < myServers.size() ; i++){
		for(size_t j = 0; j < myServers[i].server_name.size();j++){
			if(host == myServers[i].server_name[j]){
				clientHandler.setServer(myServers[i]);
				hostSeted = true;
				return ;
			}
		}
		
		std::map<std::string, std::vector<std::string> >::iterator it = myServers[i].host_port.begin();
		while (it != myServers[i].host_port.end()){
			if(host == it->first){
				clientHandler.setServer(myServers[i]);
				hostSeted = true;
				return ;
			}
			it++;
		}
			
	}
	if(!hostSeted)
		clientHandler.setServer(myServers[0]);
	hostSeted = true;
}

bool client::appendFirstRequest(const char* buf, ssize_t read)
{
	requestData.insert(requestData.end(), buf, buf + read);
	std::string requestLine(requestData.begin(), requestData.end());
	
	std::cout << "requestline: "<< requestLine << std::endl;
	size_t pos = requestLine.find("\r\n\r\n");
	if (pos != std::string::npos) {
		size_t hostPos = requestLine.find("Host: ");
		if (hostPos != std::string::npos) {
			hostPos += 6; 
			size_t endPos = requestLine.find("\r\n", hostPos);
			std::string hostName = requestLine.substr(hostPos, endPos - hostPos);
			size_t colonPos = hostName.find(':');
            if (colonPos != std::string::npos) {
                hostName = hostName.substr(0, colonPos);
            }
			setHost(hostName);
			std::cout << "found host name: " << hostName << std::endl;
			clientHandler.appendData(&requestData[0], requestData.size());
			requestData.clear(); 
			if(clientHandler.isComplete())
				return true;
		}
	}
	return false;
}




