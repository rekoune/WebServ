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

HttpStatusCode      Response::writeBodyInFile(std::string& path, std::vector<char>& body){
    HttpStatusCode  status;

    if (access(path.c_str(), F_OK) == 0)
        status = OK;
    else
        status = CREATED;
    std::ofstream file(path.c_str(), std::ios::out | std::ios::binary);
    if (!file)
        return INTERNAL_SERVER_ERROR;

    file.write(&body[0],body.size());
    return (status);
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
    // resElements.statusLine = getStatusLine();
    // resElements.headers = generateHeaders();
    // buildResponse();
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
    // resElements.statusLine = getStatusLine();
    // resElements.headers = generateHeaders();
    // buildResponse();
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
        // resElements.statusLine = getStatusLine();
        // resElements.headers = generateHeaders();
        // buildResponse();
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

HttpStatusCode     Response::getUploadPath(){
    std::string uploadPath;
    HttpStatusCode status;

    uploadPath.append(resInfo.location.root);
    uploadPath.append(resInfo.location.path);
    uploadPath.append(resInfo.location.upload_store);
    if (access(uploadPath.c_str(), F_OK) != 0)
        return INTERNAL_SERVER_ERROR;
    uploadPath.append(resInfo.path.begin() + resInfo.location.root.length() + resInfo.location.path.length(), resInfo.path.end());
    status = getPathType(uploadPath, resInfo.type);
    if (status == OK)
        resInfo.path = uploadPath;
    return (status);
}

void    Response::handelPOST(){
    std::map<std::string, std::string>::iterator it;
    std::vector<char> body;

    body = resInfo.req.getBody();
    if (body.size() > resInfo.server.client_max_body_size){
        resInfo.status = CONTENT_TOO_LARGE;
        errorHandling();
        return;
    }
    resInfo.status = getUploadPath();
    if (resInfo.status != OK){
        errorHandling();
        return;
    }
    it = resInfo.req.getHeaders().find("content-type");
    if (it == resInfo.req.getHeaders().end()){
        if (body.size() != 0){
            if (resInfo.type == F)
                resInfo.status = writeBodyInFile(resInfo.path, body);
            else if (resInfo.type == DIR_LS){
                std::string prefix("File");
                resInfo.status = writeBodyInFile(resInfo.path.append("/").append(Utils::randomName(prefix)).append(".bin"), body);
            }
            return;
        }
    };
}

void    Response::successHandling(){
    if (resInfo.method == "GET"){
        handelGET();
    }
    else if (resInfo.method == "POST"){
        handelPOST();
    }
    else if (resInfo.method == "DELETE")
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
    resElements.statusLine = getStatusLine();
    resElements.headers = generateHeaders();
    buildResponse();
    // std::cout << "status = " << resInfo.status << std::endl;
}

