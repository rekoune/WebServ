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
    headers.insert(std::pair<std::string, std::string> ("Connection", "keep-alive"));
    headers.insert(std::pair<std::string, std::string> ("Accept-Ranges", "bytes"));

    return (headers);
}

std::vector<char>   Response::getBodyFromFile(std::string& path){
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    std::map<std::string, std::string> headers = resInfo.req.getHeaders();
    std::map<std::string, std::string>::iterator it = headers.find("range");


    std::streamsize size;
    size_t startPos = 0;

    file.seekg(startPos, std::ios::end);
    size = file.tellg();
    if (it != headers.end()){
        std::string range(it->second.begin() + 6, it->second.end());
        startPos = Utils::strToNumber(range);
        if (startPos >= (size_t)size)
            startPos = 0;
        size = 2 * 1024 * 1024;
        resInfo.status = PARTIAL_CONTENT;
        std::cout << "start pos = " << startPos << " , size = " << size << " , size + range = " << size + startPos << " , total size = " << file.tellg() << std::endl;
        std::string contentRange("bytes ");
        contentRange.append(Utils::toString(startPos));
        contentRange.append("-");
        contentRange.append(Utils::toString(startPos + size));
        contentRange.append("/");
        contentRange.append(Utils::toString(file.tellg()));
        resElements.headers.insert(std::pair<std::string, std::string> ("Content-Range", contentRange));
    }

    file.seekg(startPos, std::ios::beg);
    std::vector<char> body(size);
    file.read(&body[0], size);
    std::cout << "body size = " << body.size() << std::endl;
    file.close();
    return (body);
}

HttpStatusCode      Response::writeBodyInFile(std::string& path, std::vector<char>& body){
    HttpStatusCode  status;

    // std::cout << "======================== bodysize =  ======================" << std::endl;
    // std::cout << body.size() << std::endl;
    // std::cout.write(body.data(), body.size()) << std::endl;

    if (body.empty())
        return OK;
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

    this->resElements.statusLine.clear();
    this->resElements.headers.clear();
    this->resElements.body.clear();
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
    std::string oldPath;
    std::string target;
    std::string elementName;
    
    target = resInfo.req.getRequestLine().target;
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
    // resElements.statusLine = getStatusLine();
    // resElements.headers = generateHeaders();
    // buildResponse();
}

void    Response::handleDELETE(){
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


void    Response::setFullPathByType(std::string& path, PathTypes& pathType, std::string contentType){
    if (pathType == F){
        if (contentType.empty() || contentType == Utils::getFileType(fileTypes, Utils::getFileName(path))){
            return;
        }
        else
            path.append(Utils::findExtensionByMime(fileTypes, contentType));
    }
    else if (pathType == DIR_LS){
        std::string prefix("File");
        path.append("/").append(Utils::randomName(prefix)).append(Utils::findExtensionByMime(fileTypes, contentType));
    }
}

HttpStatusCode  Response::extractHeaders(std::string bodyHeaders, std::map<std::string, std::string>& headers){
    std::stringstream   ss(bodyHeaders);
    std::string         line;
    std::string         key, value;

    std::getline(ss, line, '\n');
    while(!ss.eof() && line != "\r"){
        std::stringstream headerStream(line);
        std::getline(headerStream, key, ':');
        std::getline(headerStream, value);
        
        if ((!Utils::isBlank(key) && key.find(' ') != std::string::npos) || value.at(value.length() - 1) != '\r'){
            std::cout << "joma joma joma" << std::endl;
            return (BAD_REQUEST);
        }
        value.erase(value.length() - 1, 1);
        if (!Utils::isBlank(key) && !Utils::isBlank(value)){
            Utils::strToLower(key);
            Utils::trimSpaces(value);
            headers.insert(std::pair<std::string, std::string> (key, value));
        }
        std::getline(ss, line, '\n');
    }
    std::map<std::string, std::string>::iterator it;
    it = headers.find("content-disposition");
    if (it == headers.end() || it->second.find("form-data") == std::string::npos){
        std::cout << "9al 9al 9al" << std::endl;
        return BAD_REQUEST;
    }
    // std::cout << "*********************************** Multi Part Headers *********************************************" << std::endl;
    // std::string str = Utils::mapToString(headers);
    // std::cout.write(str.c_str(), str.length()) << std::endl;;
    // std::cout << "****************************************************************************************************" << std::endl;
    return OK;
}

HttpStatusCode  Response::handleSinglePart(std::vector<char> singlePart, size_t size){
    long    headersPos;
    std::map<std::string, std::string> headers;
    HttpStatusCode  status;
    std::vector<char> body;
    std::string path(resInfo.path);
    // std::cout << " single size = " << size << std::endl;
    // std::cout << " single size = " << singlePart.size() << std::endl;

    // // std::cout << " >>>>>>>>>>> path before : " << path << std::endl;
    // std::cout << "==================Single part =======================" << std::endl;
    // std::cout.write(singlePart.data(), size);
    // std::cout << "===========================================" << std::endl;
    headersPos = Utils::isContainStr(&singlePart[0], size, "\r\n\r\n", 4);
    if (headersPos == -1){
        std::cout << "jo9 jo9 jo9" << std::endl;
        status = BAD_REQUEST;
    }
    else{
        std::string hed(singlePart.begin(), singlePart.begin() + headersPos + 2);
        status = extractHeaders(hed, headers);
        // std::cout << "==> size = " << size << " , header pos = " << headersPos << std::endl;
        if(headersPos + 4 == (long)size)
            return OK;
        if (status == OK){
            body = std::vector<char>(singlePart.begin() + headersPos + 4, singlePart.begin() + headersPos + 4 + (size - headersPos - 4));
            std::map<std::string, std::string>::iterator contentType = headers.find("content-type");
            std::map<std::string, std::string>::iterator diposition = headers.find("content-disposition");
            std::string fileName;
            size_t      fileNamePos;
            if (diposition == headers.end() || diposition->second.find("form-data") == std::string::npos){
                std::cout << "sa7 sa7 sa7" << std::endl;
                return BAD_REQUEST;
            }
            else if ((fileNamePos = diposition->second.find("filename=")) != std::string::npos){
                path.append("/");
                path.append(diposition->second.begin() + fileNamePos +10, diposition->second.end() - 1);
            }
            else{
                std::string prefix("File");
                path.append("/");
                path.append(Utils::randomName(prefix));
            }
            if (contentType != headers.end()){
                if (contentType->second != Utils::getFileType(fileTypes, Utils::getFileName(path)))
                    path.append(Utils::findExtensionByMime(fileTypes, contentType->second));
            }
            else
                path.append(".bin");
            // std::cout << ">>>>>>>> path = " << path<< std::endl;
            status = writeBodyInFile(path, body);
        }
        else
            std::cout << "kaf kaf kaf status = " << status << std::endl;
    }
    return status;
}

HttpStatusCode  Response::handleMultiParts(const std::vector<char>& body, std::string boundary){
    if (boundary.empty()){
        std::cout << "kaa kaa kaa boundary is empty" << std::endl;
        return BAD_REQUEST;
    }
    std::string start, end;
    long currentPos, endPos, nextPos = 0;
    HttpStatusCode status;

    start.append("--").append(boundary);
    end.append(start).append("--").append("\r\n");
    currentPos = Utils::isContainStr(&body[0], body.size(), start.c_str(), start.length());
    endPos     = Utils::isContainStr(&body[0], body.size(), end.c_str(), end.length());
    if (currentPos != 0 || endPos == -1 || endPos + end.length() != body.size()){
        std::cout << "mo9 mo9 mo9" << std::endl;
        return BAD_REQUEST;
    }
    currentPos = start.length() + 2;
    while(currentPos < endPos){
        nextPos = Utils::isContainStr(&body[currentPos], body.size() - currentPos, start.c_str(), start.length());
        // std::cout << "next pos = " << nextPos << " , endpos = "  << endPos << ", current = " << currentPos << std::endl;
        status = handleSinglePart(std::vector<char>(body.begin() + currentPos, body.begin() + nextPos + currentPos), nextPos  - 2);
        if (status != OK && status != CREATED){
            std::cout << "za3 za3 za3" << std::endl;
            return status;
        }
        currentPos += nextPos + 2 + start.length();
    }
    return status;
}

HttpStatusCode Response::handleContentType(){
    std::map<std::string, std::string>::iterator it;
    std::vector<char>   body;
    HttpStatusCode      status = resInfo.status;
    std::map<std::string, std::string> headers = resInfo.req.getHeaders();

    body = resInfo.req.getBody();
    it = headers.find("content-type");
    if (it == headers.end() || it->second == "application/x-www-form-urlencoded"){
        setFullPathByType(resInfo.path, resInfo.type, "");
        status = writeBodyInFile(resInfo.path, body);
    }
    else if (it->second.find("multipart/form-data") != std::string::npos){
        size_t  BoundPos;
        if ((BoundPos = it->second.find("boundary=")) != std::string::npos && resInfo.type == DIR_LS){
            status = handleMultiParts(resInfo.req.getBody(), &it->second[BoundPos + 9]);
        }
        else
            status = BAD_REQUEST;
    }
    else{
        setFullPathByType(resInfo.path, resInfo.type, it->second);
        status = writeBodyInFile(resInfo.path, body);
    }
    return (status);
}

void    Response::handlePOST(){
    std::vector<char> body;

    body = resInfo.req.getBody();
    if (body.size() > resInfo.server.client_max_body_size){
        resInfo.status = CONTENT_TOO_LARGE;
        errorHandling();
        return;
    }
    // else if ()
    resInfo.status = getUploadPath();
    if (resInfo.status != OK){
        errorHandling();
        return;
    }
    resInfo.status = handleContentType();
    if (resInfo.status != OK && resInfo.status != CREATED)
        errorHandling();
}

void    Response::successHandling(){
    if (resInfo.method == "GET"){
        handleGET();
    }
    else if (resInfo.method == "POST"){
        handlePOST();
    }
    else if (resInfo.method == "DELETE")
    {
        handleDELETE();
    }
}

void Response::handle(){
    setFileTypes();
    if (resInfo.status != OK){
        errorHandling();
    }
    else
        successHandling();
    resElements.statusLine = getStatusLine();
    generateHeaders(resElements.headers);
    buildResponse();
    // std::cout << "status = " << resInfo.status << std::endl;
}

void Response::clear(){
    this->response.clear();
}