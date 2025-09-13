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
Response::Response(const HttpResponseInfo info){
    this->resInfo = info;
}
void Response::setResInfo(const HttpResponseInfo& info){
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

    fileTypes.insert(std::pair<std::string, std::string> ("gif", "image/gif"));
    fileTypes.insert(std::pair<std::string, std::string> ("jpg", "image/jpeg"));
    fileTypes.insert(std::pair<std::string, std::string> ("jpeg", "image/jpeg"));
    fileTypes.insert(std::pair<std::string, std::string> ("png", "image/png"));

    fileTypes.insert(std::pair<std::string, std::string> ("mp4", "video/mp4"));
    fileTypes.insert(std::pair<std::string, std::string> ("mov", "video/quicktime"));

    fileTypes.insert(std::pair<std::string, std::string> ("js", "application/javascript"));
    fileTypes.insert(std::pair<std::string, std::string> ("pdf", "application/pdf"));
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

std::map<std::string, std::string>  Response::generateHeaders(){
    std::map<std::string, std::string> headers;

    headers.insert(std::pair<std::string, std::string> ("Server", "WebServer"));
    headers.insert(std::pair<std::string, std::string> ("Date", Utils::getDate()));
    if (resInfo.status != NO_CONTENT)
        headers.insert(std::pair<std::string, std::string> ("Content-Type", Utils::getFileType(fileTypes, Utils::getFileName(resInfo.path))));
    headers.insert(std::pair<std::string, std::string> ("Content-Length", Utils::toString(resElements.body.size())));
    headers.insert(std::pair<std::string, std::string> ("Connection", "keep-alive"));

    return (headers);
}

std::vector<char>   Response::getBodyFromFile(std::string& path){
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);

    std::streamsize size;

    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> body(size);
    file.read(&body[0], size);
    file.close();
    return (body);
}

void    Response::buildResponse(){
    Utils::pushInVector(response, resElements.statusLine);
    Utils::pushInVector(response, Utils::mapToString(resElements.headers));
    response.insert(response.end(), resElements.body.begin(), resElements.body.end());
}


void    Response::errorHandling(){
    std::map<int, std::string>::iterator errorPage;
    if ((errorPage = resInfo.server.error_pages.find(resInfo.status)) != resInfo.server.error_pages.end()){
        std::string errorPath;

        errorPath.append(resInfo.location.root);
        errorPath.append("/");
        errorPath.append(errorPage->second);
        resInfo.path = errorPath;
        if (access(errorPath.c_str(), F_OK) == -1 || access(errorPath.c_str(), R_OK) == -1){
            resInfo.status = INTERNAL_SERVER_ERROR;
            resElements.body = generateErrorBody();
        }
        else
            resElements.body = getBodyFromFile(errorPath);

    }
    else
        resElements.body = generateErrorBody();
    resElements.statusLine = getStatusLine();
    resElements.headers = generateHeaders();
    buildResponse();
}

void    Response::generateListingBody(DIR* dir){
    struct dirent* dirContent;
    
    resInfo.path = "list.html";
    Utils::pushInVector(resElements.body, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n  <meta charset=\"UTF-8\">\n  <title>Directory Listing</title>\n</head>\n<body>\n <ul>\n");
    while((dirContent = readdir(dir))){
        Utils::pushInVector(resElements.body, "  <li><a href=\"");
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

void    Response::handelGET(){
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
    resElements.statusLine = getStatusLine();
    resElements.headers = generateHeaders();
    buildResponse();
}

void    Response::handelDELETE(){
    if (resInfo.type == DIR_LS){
        resInfo.status = FORBIDDEN;
        errorHandling();
    }
    else if (std::remove(resInfo.path.c_str()) != 0){
        resInfo.status = INTERNAL_SERVER_ERROR;
        errorHandling();
    }
    else{
        resInfo.status = NO_CONTENT;
        resElements.statusLine = getStatusLine();
        resElements.headers = generateHeaders();
        buildResponse();
    }
}

void    Response::handelPOST(){
    // if (resInfo.req.getBody().empty()){
    //     std::cout << "hos hos hos" << std::endl;
    //     resInfo.status = BAD_REQUEST;
    //     errorHandling();
    // }
     if (resInfo.req.getBody().size() > resInfo.server.client_max_body_size){
        resInfo.status = CONTENT_TOO_LARGE;
        errorHandling();
    }
}

void    Response::successHandling(){
    if (resInfo.req.getRequestLine().method == "GET"){
        handelGET();
    }
    else if (resInfo.req.getRequestLine().method == "POST"){
        handelPOST();
    }
    else if (resInfo.req.getRequestLine().method == "DELETE")
    {
        handelDELETE();
    }
}

void Response::handel(){
    setFileTypes();
    if (resInfo.status != OK){
        errorHandling();
    }
    else
        successHandling();

}

