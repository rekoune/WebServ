# include "../../../includes/ResourceResolver.hpp"

ResourceResolver::ResourceResolver(){};
ResourceResolver::~ResourceResolver(){};

ResourceResolver::ResourceResolver(const RequestLine& reqLine, const std::vector<LocationConfig>& locations){
    this->reqLine = reqLine;
    this->locations = locations;
}
ResourceResolver::ResourceResolver(const ResourceResolver& other){
    *this = other;
}

ResourceResolver& ResourceResolver::operator=(const ResourceResolver& other){
    this->reqLine = other.reqLine;
    this->locations = other.locations;
    this->resInfo = other.resInfo;
    return (*this);
}

void ResourceResolver::setRequestLine(const RequestLine& req){
    this->reqLine = req;
}
void ResourceResolver::setLocations(const std::vector<LocationConfig>& locations){
    this->locations = locations;
}
void ResourceResolver::setServer(const ServerConfig&    server){
    this->resInfo.server = server;
}

HttpResourceInfo ResourceResolver::getResponseInfo() const {
    return (this->resInfo);
}

HttpStatusCode ResourceResolver::findLocation(std::vector<LocationConfig> locations, std::string& reqTarget, LocationConfig& resultLocation){
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
        return NOT_FOUND;
    }
    resultLocation = locations.at(matchedIndex);
    if (!resultLocation.redirection_url.empty() && resInfo.method == "GET"){
        resInfo.path = resultLocation.redirection_url;
        return (static_cast<HttpStatusCode>(resultLocation.redirection_status));
    }
    return (OK);
}

HttpStatusCode  ResourceResolver::isMethodAllowed(std::vector<std::string> allowedMethods, std::string reqMethod){
    if (allowedMethods.empty() && reqMethod == "GET")
        return (OK);
    for(size_t i = 0; i < allowedMethods.size(); i++){
        if (allowedMethods.at(i) == reqMethod)
            return (OK);
    }
    return (METHOD_NOT_ALLOWED);
}

HttpStatusCode ResourceResolver::dirHandling(std::string& path, PathTypes& pathType, LocationConfig& location){
    std::string temp(path);
    const char* c_path;
    struct stat type;


    if (resInfo.method == "GET"){
        temp.append("/");
        temp.append(location.index[0]);
        c_path = temp.c_str();
        if (stat(c_path, &type) != -1 && S_ISREG(type.st_mode)){
            path = temp;
            pathType = F;
            return (OK);
        }
    }
    if (location.autoindex || resInfo.method == "POST")
        pathType = DIR_LS;
    else
        return (FORBIDDEN);
   return (OK);
}


HttpStatusCode ResourceResolver::fileHandling(std::string& path, PathTypes& pathType){
    if (Utils::isScript(path, resInfo.location.cgi_extension)){
        pathType = SCRIPT;
        return (OK);
    }
    pathType = F;
    return (OK);
}

HttpStatusCode ResourceResolver::resolveResourceType(std::string& path, PathTypes& pathType, LocationConfig& location){
    const char * c_path;
    struct stat type;
    HttpStatusCode status;

    c_path = path.c_str();
    if (stat(c_path, &type) == -1){
        if (this->reqLine.method == "POST"){
            pathType = F;
            return (OK);
        }
        return (NOT_FOUND);
    }
    std::map<std::string, std::string>::iterator it = this->resInfo.headers.find("content-type");
    if (S_ISREG(type.st_mode) && it != resInfo.headers.end() && it->second.find("multipart/form-data") != std::string::npos){
        return (BAD_REQUEST);
    }
    if (S_ISDIR(type.st_mode))
        status = dirHandling(path, pathType, location);
    else if (S_ISREG(type.st_mode))
        status = fileHandling(path, pathType);
    else
        return (FORBIDDEN);
    return (status);
}

HttpResourceInfo ResourceResolver::handle(std::map<std::string, std::string> headers){
    LocationConfig location;
    HttpStatusCode status;
    PathTypes   pathType;
    std::string path;

    
    this->resInfo.method = reqLine.method;
    status = findLocation(locations, reqLine.target, location);
    this->resInfo.reqLine = reqLine;
    this->resInfo.headers = headers;
    if (status == OK)
        status = isMethodAllowed(location.allowed_methods, reqLine.method);
    this->resInfo.location = location;
    if (status == OK){
        path = location.root;

        if (path.at(path.length() -1) == '/')
            path.erase(path.end() -1);
        path.append(reqLine.target);
        if (location.path != "/")
            path.erase(path.begin() + location.root.length() , path.begin() + location.path.length() + location.root.length());
        status = resolveResourceType(path, pathType, location);
        this->resInfo.type = pathType;
        this->resInfo.path = path;
    }
    this->resInfo.status = status;
    return (this->resInfo);
}
