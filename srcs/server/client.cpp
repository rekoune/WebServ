#include "../../includes/client.hpp"

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
		cgiFd = other.cgiFd;
	}
	return *this;
}


client::client(std::vector<ServerConfig>& myservers, int fd) : myServers(myservers) ,hostSeted(false),
		responseComplete(true) ,totalsend(0),totalrecv(0) ,fd(fd) ,cgiFd(-1)
{
	if(myServers.size() == 1){
		clientHandler.setServer(myservers[0]);
		myServers.clear();
		hostSeted = true;
	}
}

client::client(const client& other): 
		clientHandler(other.clientHandler),myServers(other.myServers) ,requestData(other.requestData),
		response(other.response) ,hostSeted(other.hostSeted),responseComplete(other.responseComplete) ,
		totalsend(other.totalsend),totalrecv(other.totalrecv) , fd(other.fd), cgiFd(other.cgiFd) {}


ssize_t client::ft_recv(short& event){
	char buf[BUFFER];
	size_t total = 0;


	ssize_t read = recv(fd, buf, sizeof(buf), 0);
	if(read == -1){
		std::cerr << "Error receiving data: " << strerror(errno) << std::endl; 
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
		totalrecv += read;
		total = totalrecv;
		if(clientHandler.isComplete()){
			//chekc for cgi
			cgiFd = -1; // her the function that give's the cgi fd return ;
			std::cout << "all has been receved" << std::endl;
			totalrecv = 0;
 			event = POLLOUT;
		}
	}
	// std::cout << "\033[95mthe recevide is :\n" << buf << "\033[0m" << std::endl;
	std::cout << "total recv is on fd: ("<< fd  << ") is : " << total << "B" <<  std::endl;

	return read;
}

ssize_t client::sending(short& event){
	ssize_t nsend;

	std::cout << "resopons size = " << response.size() << "total send =" << totalsend << std::endl;
	nsend = send(fd, &response[0] + totalsend, response.size() - totalsend, 0);
	if(nsend == -1){
		std::cerr << "send error: " << strerror(errno) << std::endl;
		return 0;
	}
	totalsend += nsend;
	// std::cout << "\033[95mSend: " << std::string(response.begin() + totalsend - nsend, response.begin() + totalsend).substr(0, 256) << (nsend > 99 ? "..." : "") << "\033[0m" << std::endl;
	// std::cout << "\033[95mSend: " << std::string(response.begin() + totalsend - nsend, response.begin() + totalsend) << "\033[0m" << std::endl;
	std::cout << "total data from Post of fd:  ("<< fd << ") is :" << totalsend << "B" << std::endl;
	if(totalsend == response.size()){
		if(!clientHandler.isKeepAlive())
			return 0;
		responseComplete = true;
		if(clientHandler.isResDone())
			event = POLLIN;
		totalsend = 0;
		response.clear();
		std::cout << "succufly send everyting !!!" << std::endl;
	}
	return nsend;
}

int client::cgiRun(){
	response = clientHandler.getResponse();	
	if(!response.empty()){
		responseComplete = false;
		cgiFd = -1;
	}
	return cgiFd;
}

ssize_t client::ft_send(short& event){
	if(responseComplete){
		response = clientHandler.getResponse();
		responseComplete = false;
	}
	std::cout << "sending... " << std::endl;
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
				myServers.clear();
				hostSeted = true;
				return ;
			}
		}
		std::map<std::string, std::vector<std::string> >::iterator it = myServers[i].host_port.begin();
		while (it != myServers[i].host_port.end()){
			if(host == it->first){
				clientHandler.setServer(myServers[i]);
				hostSeted = true;
				myServers.clear();
				return ;
			}
			it++;
		}	
	}
	if(!hostSeted)
		clientHandler.setServer(myServers[0]);
	myServers.clear();
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
            hostName = hostName.substr(0, colonPos);
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

int client::getCgiFd(){
	return cgiFd;
}

void client::resetCgiFd(){
	cgiFd = -1;
}

