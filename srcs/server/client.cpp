#include "../../includes/client.hpp"

void printingserver(const ServerConfig& servers){
		std::cout << "rooot : " << servers.root << std::endl;
	std::cout << "location : " << servers.locations.size() << std::endl;
	std::cout << "server_name : " << servers.server_name[0] << std::endl;
}



client::client(std::vector<ServerConfig>& myServers, int fd) :myServers(myServers) ,hostSeted(false), fd(fd){
	if(myServers.size() == 1){
		// std::cout << "host seted cause it's only one " << std::endl;
		// std::cout << "server:: " << std::endl;
		// std::cout << myServers[0].root << std::endl;
		// std::cout << myServers[0].locations[0].path << std::endl;
		std::cout << "this is clinet with fd : " << fd << std::endl;
		socketHttp.setServer(myServers[0]);
		socketHttp.server.server_name[0] = "server_name";
		std::cout << "fromt he's side"<< std::endl;
		printingserver(socketHttp.server);

		std::cout << "the server that the clinet get : " << std::endl;
		printingserver(myServers[0]);
		hostSeted = true;
	}
}
client::client(const client& other): myServers(other.myServers), hostSeted(other.hostSeted), fd(other.fd) {}


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
				socketHttp.setServer(myServers[i]);
				hostSeted = true;
				return ;
			}
		}
		std::map<std::string, std::vector<std::string> >::iterator it = myServers[i].host_port.begin();
		while (it != myServers[i].host_port.end()){
			if(host == it->first){
				socketHttp.setServer(myServers[i]);
				hostSeted = true;
				return ;
			}
			it++;
		}
			
	}
}

bool client::appendFirstRequest(const char* buf, int read)
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
			socketHttp.appendData(&requestData[0], requestData.size());
			return true;
		}
	}
	return false;
}

void client::appendData(const char *data, size_t size){

	std::cout << "this is clinet with fd : " << fd << std::endl;	
	std::cout << "printfing the server before  ===== append" << std::endl;
	printingserver(socketHttp.server);
	socketHttp.appendData(data, size);
	std::cout << "printfing the server after  ===== append" << std::endl;
	printingserver(socketHttp.server);
}

bool client::isComplete(){
	return socketHttp.isComplete();
}

std::vector<char> client::getResponse(){
	return socketHttp.getResponse();
}


