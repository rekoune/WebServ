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
    this->resInfo = other.resInfo;
    return (*this);
}

void RequestHandler::setRequest(const Request& req){
    this->req = req;
}
void RequestHandler::setServer(const ServerConfig& server){
    this->server = server;
}

HttpResponseInfo RequestHandler::getResponseInfo() const {
    return (this->resInfo);
}

HttpStatusCode RequestHandler::findLocation(std::vector<LocationConfig> locations, std::string reqTarget, LocationConfig& resultLocation){
    int matchedLenght = -1;
    int matchedIndex = -1;

    for(size_t i = 0; i < locations.size(); i++){
        if(Utils::isStartWith(reqTarget, locations.at(i).path)){
            if (matchedLenght < (int)locations.at(i).path.length()){
                matchedLenght = locations.at(i).path.length();
                matchedIndex = i;
            }
        }
    }
    if (matchedIndex == -1){
        return (NOT_FOUND);
    }
    resultLocation = locations.at(matchedIndex);
    return (OK);
}

HttpStatusCode  RequestHandler::isMethodAllowed(std::vector<std::string> allowedMethods, std::string reqMethod){
    if (allowedMethods.empty() && reqMethod == "GET")
        return (OK);
    for(size_t i = 0; i < allowedMethods.size(); i++){
        if (allowedMethods.at(i) == reqMethod)
            return (OK);
    }
    return (METHOD_NOT_ALLOWED);
}

HttpStatusCode RequestHandler::dirHandling(std::string& path, PathTypes& pathType, LocationConfig& location){
    std::string temp(path);
    const char* c_path;
    struct stat type;


    if (resInfo.method == "GET"){
        temp.append("/");
        temp.append(location.index);
        c_path = temp.c_str();
        if (stat(c_path, &type) != -1 && S_ISREG(type.st_mode)){
            path = temp;
            pathType = F;
            return (OK);
        }
    }
    if (location.autoindex || resInfo.method == "POST")
        pathType = DIR_LS;
    else{
        std::cout << "hup hup hup " << std::endl;
        return (FORBIDDEN);
    }
   return (OK);
}

bool RequestHandler::isScript(std::string& path, LocationConfig& location){
    (void)path;
    (void)location;
    return false;
}

HttpStatusCode RequestHandler::fileHandling(std::string& path, PathTypes& pathType, LocationConfig& location){
    if (isScript(path, location)){
        pathType = SCRIPT;
        return (OK);
    }
    pathType = F;
    return (OK);
}

HttpStatusCode RequestHandler::resolveResourceType(std::string& path, PathTypes& pathType, LocationConfig& location){
    const char * c_path;
    struct stat type;
    HttpStatusCode status;

    c_path = path.c_str();
    if (stat(c_path, &type) == -1){
        if (this->req.getRequestLine().method == "POST"){
            pathType = F;
            return (OK);
        }
        return (NOT_FOUND);
    }
    if (S_ISDIR(type.st_mode))
        status = dirHandling(path, pathType, location);
    else if (S_ISREG(type.st_mode))
        status = fileHandling(path, pathType, location);
    else{
        std::cout << "ha9 ha9 ha9" << std::endl;
        return (FORBIDDEN);
    }
    return (status);
}

HttpResponseInfo RequestHandler::handle(){
    LocationConfig location;
    HttpStatusCode status;
    PathTypes   pathType;
    std::string path;

    status = findLocation(server.locations, req.getRequestLine().target, location);
    this->resInfo.method = req.getRequestLine().method;
    this->resInfo.req = req;
    this->resInfo.server = server;
    if (status == OK){
        //delete
        location.allowed_methods.push_back("GET");
        location.allowed_methods.push_back("POST");
        location.allowed_methods.push_back("DELETE");
        status = isMethodAllowed(location.allowed_methods, req.getRequestLine().method);
    }
    if (status == OK){
        path = location.root;

        if (path.at(path.length() -1) == '/')
            path.erase(path.end() -1);
        path.append(req.getRequestLine().target);
        status = resolveResourceType(path, pathType, location);
    }
    this->resInfo.path = path;
    this->resInfo.type = pathType;
    this->resInfo.status = status;
    this->resInfo.location = location;
    return (this->resInfo);
}
