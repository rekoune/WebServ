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
		clientLastActivity =other.clientLastActivity;
	}
	return *this;
}


client::client(std::vector<ServerConfig>& myservers, int fd) : myServers(myservers) ,hostSeted(false),
		responseComplete(true) ,totalsend(0),totalrecv(0) ,fd(fd) ,cgiFd(-1)
{
	clientLastActivity = std::time(NULL);
	if(myServers.size() == 1){
		clientHandler.setServer(myservers[0]);
		myServers.clear();
		hostSeted = true;
	}
}

client::client(const client& other): 
		clientHandler(other.clientHandler),myServers(other.myServers) ,requestData(other.requestData),
		response(other.response) ,hostSeted(other.hostSeted),responseComplete(other.responseComplete) ,
		totalsend(other.totalsend),totalrecv(other.totalrecv) , fd(other.fd), cgiFd(other.cgiFd), clientLastActivity(other.clientLastActivity) {}


ssize_t client::ft_recv(short& event)
{
	char buf[BUFFER];
	size_t total = 0;

	ssize_t read = recv(fd, buf, sizeof(buf), 0);
	totalrecv += read;
	setupLastActivity();
	if(read == -1){
		std::cerr << "Failed to receive data on fd (" << fd << ")" << std::endl;
		return 0;
	}
	else if(!isHostSeted()){
		total = totalrecv;
		if(appendFirstRequest(buf, read)){
			cgiFd = clientHandler.isScript();
			totalrecv = 0;
			event = POLLOUT;
		}
	}
	else if(read){
		clientHandler.appendData(buf, read);
		total = totalrecv;
		if(clientHandler.isComplete()){
			cgiFd = clientHandler.isScript();
			totalrecv = 0;
 			event = POLLOUT;
		}
	}
	return read;
}


ssize_t client::sending(short& event){
	ssize_t nsend;
	nsend = send(fd, &response[0] + totalsend, response.size() - totalsend, 0);
	setupLastActivity();
	if(nsend == -1){
		std::cerr << "Failed to send data on fd (" << fd << ")" << std::endl;
		return 0;
	}
	totalsend += nsend;
	if(totalsend == response.size()){
		if(!clientHandler.isKeepAlive())
			return 0;
		responseComplete = true;
		if(clientHandler.isResDone())
			event = POLLIN;
		totalsend = 0;
		response.clear();
	}
	return nsend;
}

ssize_t client::ft_send(short& event){
	if(responseComplete){
		response = clientHandler.getResponse();
		responseComplete = false;
	}
	return sending(event);
}

void client::setErrorResponse(HttpStatusCode statuCode){
	response = clientHandler.getStatusResponse(statuCode);
	responseComplete = false;
}

void client::startTimer(){
	cgiStartTime = std::time(NULL);
}


void client::setupLastActivity(){
	clientLastActivity = std::time(NULL);
}

bool client::clientTimeOut(){
	if((std::time(NULL) - clientLastActivity) >= CLIENT_TIMEOUT)
		return true;
	return false;
}

bool client::cgiTimeOut()
{
	if((std::time(NULL) - cgiStartTime) >= CGI_TIMEOUT)
		return true;
	return false;
}

int client::cgiRun(){
	response = clientHandler.getResponse();	
	if(!response.empty()){
		responseComplete = false;
		cgiFd = -1;
	}
	return cgiFd;
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
	}
	for(size_t i = 0; i < myServers.size() ; i++){
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

void client::cgiCleaner(){
	clientHandler.cleanCGI();
}
