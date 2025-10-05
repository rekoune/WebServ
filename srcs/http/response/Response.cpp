# include "../../../includes/Response.hpp"

Response::Response(){};
Response::~Response(){};
Response::Response(const Response& other){
    *this = other;
}
Response& Response::operator=(const Response& other){
    this->resInfo = other.resInfo;
    this->resElements = other.resElements;
    this->response = other.response;
    this->fileTypes = other.fileTypes;
    return (*this);
}
Response::Response(const HttpResourceInfo info){
    this->resInfo = info;
}
void Response::setResInfo(const HttpResourceInfo& info){
    this->resInfo = info;
}
std::vector<char> Response::getResponse () const {
    return (this->response);
}

void    Response::setFileTypes(){
    fileTypes.insert(std::pair<std::string, std::string> ("html", "text/html"));
    fileTypes.insert(std::pair<std::string, std::string> ("htm", "text/html"));
    fileTypes.insert(std::pair<std::string, std::string> ("css", "text/css"));
    fileTypes.insert(std::pair<std::string, std::string> ("txt", "text/plain"));
    fileTypes.insert(std::pair<std::string, std::string> ("xml", "text/xml"));

    fileTypes.insert(std::pair<std::string, std::string> ("gif", "image/gif"));
    fileTypes.insert(std::pair<std::string, std::string> ("jpg", "image/jpeg"));
    fileTypes.insert(std::pair<std::string, std::string> ("jpeg", "image/jpeg"));
    fileTypes.insert(std::pair<std::string, std::string> ("png", "image/png"));
    fileTypes.insert(std::pair<std::string, std::string> ("jng", "image/x-jng"));
    fileTypes.insert(std::pair<std::string, std::string> ("webp", "image/webp"));

    fileTypes.insert(std::pair<std::string, std::string> ("mp4", "video/mp4"));
    fileTypes.insert(std::pair<std::string, std::string> ("mov", "video/quicktime"));
    fileTypes.insert(std::pair<std::string, std::string> ("webm", "video/webm"));

    fileTypes.insert(std::pair<std::string, std::string> ("mp3", "audio/mpeg"));

    fileTypes.insert(std::pair<std::string, std::string> ("js", "application/javascript"));
    fileTypes.insert(std::pair<std::string, std::string> ("pdf", "application/pdf"));
    fileTypes.insert(std::pair<std::string, std::string> ("json", "application/json"));
    fileTypes.insert(std::pair<std::string, std::string> ("doc", "application/msword"));
    fileTypes.insert(std::pair<std::string, std::string> ("rar", "application/x-rar-compressed"));
    fileTypes.insert(std::pair<std::string, std::string> ("zip", "application/zip"));
    fileTypes.insert(std::pair<std::string, std::string> ("bin", "application/octet-stream"));

}

std::string Response::getStatusLine(){
    std::string statusLine;

    statusLine.append("HTTP/1.1 ");
    statusLine.append(Utils::toString(resInfo.status));
    statusLine.append(" ");
    statusLine.append(getStatusMessage(resInfo.status));
    statusLine.append("\r\n");

    return (statusLine);
}

std::string         Response::getStatusMessage(HttpStatusCode status){
    switch (status)
    {
        case CONTINUE:
            return ("CONTINUE");
        case OK:
            return ("OK");
        case PARTIAL_CONTENT:
            return ("PARTIAL_CONTENT");
        case CREATED:
            return ("CREATED");
        case NO_CONTENT:
            return ("NO_CONTENT");
        case BAD_REQUEST:
            return ("BAD_REQUEST");
        case UNAUTHORIZED:
            return ("UNAUTHORIZED");
        case FORBIDDEN:
            return ("FORBIDDEN");
        case NOT_FOUND:
            return ("NOT_FOUND");
        case METHOD_NOT_ALLOWED:
            return ("METHOD_NOT_ALLOWED");
        case CONTENT_TOO_LARGE:
            return ("CONTENT_TOO_LARGE");
        case INTERNAL_SERVER_ERROR:
            return ("INTERNAL_SERVER_ERROR");
        case NOT_IMPLEMENTED:
            return ("NOT_IMPLEMENTED");
        case HTTP_VERSION_NOT_SUPPORTED:
            return ("HTTP_VERSION_NOT_SUPPORTED");
    }
    return "DEFAUTL";
}

std::vector<char>  Response::generateErrorBody(){
    std::stringstream bodyStream;
    std::vector<char> body;

    resInfo.path = "error.html";
    bodyStream << "<html>\n<head><title>";
    bodyStream << Utils::toString(this->resInfo.status);
    bodyStream << " ";
    bodyStream << getStatusMessage(this->resInfo.status);
    bodyStream << "</title></head>\n<body>\n<center><h1>";
    bodyStream << Utils::toString(this->resInfo.status);
    bodyStream << " ";
    bodyStream << getStatusMessage(this->resInfo.status);
    bodyStream << "</h1></center>\n<hr><center>WebServer</center>\n</body>\n</html>\n";
    Utils::pushInVector(body, bodyStream.str());
    return (body);
}

std::map<std::string, std::string>  Response::generateHeaders(std::map<std::string, std::string>& headers){

    headers.insert(std::pair<std::string, std::string> ("Server", "WebServer"));
    headers.insert(std::pair<std::string, std::string> ("Date", Utils::getDate()));
    if (resInfo.status != NO_CONTENT)
        headers.insert(std::pair<std::string, std::string> ("Content-Type", Utils::getFileType(fileTypes, Utils::getFileName(resInfo.path))));
    headers.insert(std::pair<std::string, std::string> ("Content-Length", Utils::toString(resElements.body.size())));
    if (resInfo.status != OK && resInfo.status != CREATED)
        headers.insert(std::pair<std::string, std::string> ("Connection", "close"));
    else
        headers.insert(std::pair<std::string, std::string> ("Connection", "keep-alive"));
    if (resInfo.method == "GET")
        headers.insert(std::pair<std::string, std::string> ("Accept-Ranges", "bytes"));

    return (headers);
}

std::vector<char>   Response::getBodyFromFile(std::string& path){
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    std::map<std::string, std::string> headers = resInfo.headers;
    std::map<std::string, std::string>::iterator it = headers.find("range");


    std::streamsize size;
    size_t startPos = 0;
    std::string range;

    file.seekg(startPos, std::ios::end);
    size = Utils::getFileSize(path);
    if (it != headers.end()){
        range = std::string (it->second.begin() + 6, it->second.end());
        resInfo.status = PARTIAL_CONTENT;
    }
    startPos = Utils::strToNumber(range);
    if (startPos >= (size_t)size)
        startPos = 0;
    if (size > 30 * 1024 * 1024)
        size = 5 * 1024 * 1024; //to kb
    std::cout << "start pos = " << startPos << " , size = " << size << " , size + range = " << size + startPos << " , total size = " << Utils::getFileSize(path) << std::endl;
    std::string contentRange("bytes ");
    contentRange.append(Utils::toString(startPos));
    contentRange.append("-");
    contentRange.append(Utils::toString(startPos + size));
    contentRange.append("/");
    contentRange.append(Utils::toString(file.tellg()));
    resElements.headers.insert(std::pair<std::string, std::string> ("Content-Range", contentRange));
    // }

    file.seekg(startPos, std::ios::beg);
    std::vector<char> body(size);
    file.read(&body[0], size);
    std::cout << "body size = " << body.size() << std::endl;
    file.close();
    return (body);
}

HttpStatusCode      Response::writeBodyInFile(std::string& path, std::vector<char>& body){
    HttpStatusCode  status;

    if (body.empty())
        return OK;
    if (access(path.c_str(), F_OK) == 0)
        status = OK;
    else
        status = CREATED;
    std::ofstream file(path.c_str(), std::ios::out | std::ios::binary);
    if (!file){
        std::cout << "jfj jfj jfj " << std::endl;
        return INTERNAL_SERVER_ERROR;
    }
    file.write(&body[0],body.size());
    return (status);
}

void    Response::buildResponse(){
    Utils::pushInVector(response, resElements.statusLine);
    Utils::pushInVector(response, Utils::mapToString(resElements.headers));
    response.insert(response.end(), resElements.body.begin(), resElements.body.end());

    this->resElements.statusLine.clear();
    this->resElements.headers.clear();
    this->resElements.body.clear();
}


void    Response::errorHandling(){
    std::map<int, std::string>::iterator errorPage;
    if ((errorPage = resInfo.server.error_pages.find(resInfo.status)) != resInfo.server.error_pages.end()){
        std::string errorPath;

        errorPath.append(resInfo.location.root);
        // errorPath.append("/");
        errorPath.append(errorPage->second);
        resInfo.path = errorPath;
        std::cout << "error path = " << errorPath << std::endl;
        if (access(errorPath.c_str(), F_OK) == -1 || access(errorPath.c_str(), R_OK) == -1){
            std::cout << "qwr qwr qwr" << std::endl;
            resInfo.status = NOT_FOUND;
            resElements.body = generateErrorBody();
        }   
        else
            resElements.body = getBodyFromFile(errorPath);

    }
    else
        resElements.body = generateErrorBody();
}

void    Response::generateListingBody(DIR* dir){
    struct dirent* dirContent;
    std::string oldPath;
    std::string target;
    std::string elementName;
    
    target = resInfo.reqLine.target;
    if (target.at(target.length() -1) != '/')
        target.append("/");
    resInfo.path = "list.html";
    Utils::pushInVector(resElements.body, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n  <meta charset=\"UTF-8\">\n  <title>Directory Listing</title>\n</head>\n<body>\n <ul>\n");
    while((dirContent = readdir(dir))){
        elementName = dirContent->d_name;
        if (elementName == "." || elementName == "..")
            continue;
        Utils::pushInVector(resElements.body, "  <li><a href=\"");
        Utils::pushInVector(resElements.body, target);
        Utils::pushInVector(resElements.body, dirContent->d_name);
        Utils::pushInVector(resElements.body, "\">");
        Utils::pushInVector(resElements.body, dirContent->d_name);
        Utils::pushInVector(resElements.body, "</a></li>\n");
    }
    Utils::pushInVector(resElements.body, " </ul>\n</body>\n</html>\n");
}

void    Response::listDirectory(){
    DIR* dir;

    dir = opendir(resInfo.path.c_str());
    if (dir == NULL){
        resInfo.status = FORBIDDEN;
        errorHandling();
        return;
    }
    generateListingBody(dir);
    closedir(dir);
}

void    Response::handleGET(){
    if (resInfo.type == F){
        if (access(resInfo.path.c_str(), R_OK) == -1)
        {
            resInfo.status = FORBIDDEN;
            errorHandling();
        }
        resElements.body = getBodyFromFile(resInfo.path);
    }
    else if (resInfo.type == DIR_LS){
        listDirectory();
    }
    else if (resInfo.type == SCRIPT){
        std::cout << "The request Target Is A Script" << std::endl;
    }
}

void    Response::handleDELETE(){
    if (resInfo.type == DIR_LS){
        resInfo.status = FORBIDDEN;
        errorHandling();
    }
    else if (std::remove(resInfo.path.c_str()) != 0){
        std::cout << "plm plm plm " << std::endl;
        resInfo.status = INTERNAL_SERVER_ERROR;
        errorHandling();
    }
    else{
        resInfo.status = NO_CONTENT;
    }
}

HttpStatusCode    Response::getPathType(std::string path, PathTypes& type){
    struct stat     statType;
    long            pos;
    char            lastChar;
    HttpStatusCode  status = NOT_FOUND;

    if (access(path.c_str(), F_OK) == 0){
        stat(path.c_str(), &statType);
        if (S_ISREG(statType.st_mode))
            type = F;
        else if (S_ISDIR(statType.st_mode))
            type = DIR_LS;
        else
            return (FORBIDDEN);
        return (OK);
    }
    pos = (long)path.length() - 1;
    for(; pos >= 0; pos--)
        if (path.at(pos) == '/')
            break;
    lastChar = path.at(path.length() -1);
    path.erase(path.begin() + pos, path.end());
    if (stat(path.c_str(), &statType) != -1){
        if(S_ISDIR(statType.st_mode)){
            if (lastChar == '/')
                type = DIR_LS;
            else
                type = F;
            status = OK;
        }
    }
    return status;
}

void    Response::successHandling(){
    if (resInfo.method == "GET"){
        handleGET();
    }
    else if (resInfo.method == "POST"){
        return;
    }
    else if (resInfo.method == "DELETE")
    {
        handleDELETE();
    }
}

void Response::handle(){
    setFileTypes();
    if (resInfo.status != OK && resInfo.status != CREATED){
        std::cout << "in response handle status = " << resInfo.status << std::endl;
        errorHandling();
    }
    else
        successHandling();
    resElements.statusLine = getStatusLine();
    generateHeaders(resElements.headers);
    buildResponse();
}

void Response::clear(){
    this->response.clear();
}