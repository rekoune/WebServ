#include "../../includes/client.hpp"


client::client(std::vector<ServerConfig>& myServers) :myServers(myServers) ,hostSeted(false){
	if(myServers.size() == 1){
		socketHttp.setServer(myServers[0]);
		hostSeted = true;
	}
}

bool client::isHostSeted(){
	return hostSeted;
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

void client::appendFirstRequest(const char* buf, int read)
{
	requestData.insert(requestData.end(), buf, buf + read);

	std::string requestLine(requestData.begin(), requestData.end());
	size_t pos = requestLine.find("\r\n\r\n");
	if (pos != std::string::npos) {
		size_t hostPos = requestLine.find("Host: ");
		if (hostPos != std::string::npos) {
			hostPos += 6; 
			size_t endPos = requestLine.find("\r\n", hostPos);
			hostName = requestLine.substr(hostPos, endPos - hostPos);
			size_t colonPos = hostName.find(':');
            if (colonPos != std::string::npos) {
                hostName = hostName.substr(0, colonPos);
            }
			setHost(hostName);
		}
	}
}

void client::appendData(const char *data, size_t size){
	socketHttp.appendData(data, size);
}

bool client::isComplete(){
	return socketHttp.isComplete();
}

std::vector<char> client::getResponse(){
	return socketHttp.getResponse();
}


