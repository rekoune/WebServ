# include "../../../includes/RequestHandler.hpp"

RequestHandler::RequestHandler(){};
RequestHandler::~RequestHandler(){};

RequestHandler::RequestHandler(const Request& req, const ServerConfig& server){
    this->req = req;
    this->server = server;
}
RequestHandler::RequestHandler(const RequestHandler& other){
    *this = other;
}

RequestHandler& RequestHandler::operator=(const RequestHandler& other){
    this->req = other.req;
    this->server = other.server;
    return (*this);
}

void RequestHandler::setRequest(const Request& req){
    this->req = req;
}
void RequestHandler::setServer(const ServerConfig& server){
    this->server = server;
}


HttpStatusCode RequestHandler::findLocation(std::vector<LocationConfig> locations, std::string reqTarget, LocationConfig& resultLocation){
    int matchedLenght = -1;
    int matchedIndex = -1;

    for(size_t i = 0; i < locations.size(); i++){
        std::cout << "req targit = " << reqTarget << ", location = " << locations[i].path << std::endl;
        if(Utils::isStartWith(reqTarget, locations.at(i).path)){
            if (matchedLenght < (int)locations.at(i).path.length()){
                matchedLenght = locations.at(i).path.length();
                matchedIndex = i;
            }
        }
    }
    if (matchedIndex == -1)
        return (NOT_FOUND);
    resultLocation = locations.at(matchedIndex);
    return (OK);
}

HttpStatusCode RequestHandler::handle(){
    LocationConfig location;
    HttpStatusCode status;

    status = findLocation(server.locations, req.getRequestLine().target, location);
    std::cout << "location = " << location.path << std::endl;

    return (status);
}
