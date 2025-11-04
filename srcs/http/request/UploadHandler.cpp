# include "../../../includes/UploadHandler.hpp"

UploadHandler::UploadHandler(): uploadSize(-1){
    currentState = SEARCHING_BOUNDARY;
    currentTotalSize = 0;
    setFileTypes();
};
UploadHandler::~UploadHandler(){};

UploadHandler::UploadHandler(const UploadHandler& other): uploadSize(-1){
    setFileTypes();
    currentTotalSize = 0;
    currentState = SEARCHING_BOUNDARY;
    *this = other;
};

UploadHandler& UploadHandler::operator=(const UploadHandler& other){
    this->resInfo = other.resInfo;
    this->bodySaver = other.bodySaver;
    this->currentState = other.currentState;
    this->uploadPath = other.uploadPath;
    this->parseState = other.parseState;
    this->contentType = other.contentType;
    this->boundary = other.boundary;
    this->fileTypes = other.fileTypes;
    this->uploadSize = other.uploadSize;
    return (*this);
}

UploadHandler::UploadHandler(HttpResourceInfo& resInfo) : uploadSize(-1){
    setFileTypes();
    this->resInfo = resInfo;
    currentTotalSize = 0;
    currentState = SEARCHING_BOUNDARY;
}
void UploadHandler::setResInfo(const HttpResourceInfo& resInfo){
    this->resInfo = resInfo;
}
void            UploadHandler::setParseState(const ParseState& parseState){
    this-> parseState = parseState;
}
void            UploadHandler::setUploadSize(const long long& uploadSize){
    if (parseState == PARSE_BODY_LENGTH)
        this-> uploadSize = uploadSize;
}

HttpResourceInfo    UploadHandler::getResourseInfo( void ) const{
        return (resInfo);
}

void    UploadHandler::setFileTypes(){
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
    fileTypes.insert(std::pair<std::string, std::string> ("heic", "image/heic"));

    fileTypes.insert(std::pair<std::string, std::string> ("mp4", "video/mp4"));
    fileTypes.insert(std::pair<std::string, std::string> ("MP4", "video/mp4"));
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

void    UploadHandler::clearFiles(std::vector<std::string>& files){
    for(size_t i = 0; i < files.size(); i++){
        std::remove(files.at(i).c_str());
    }
}

HttpStatusCode    UploadHandler::getPathType(std::string path, PathTypes& type){
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


void    UploadHandler::setFullPathByType(std::string& path, PathTypes& pathType, std::string contentType){
    if (pathType == F){
        if (Utils::isScript(path, resInfo.location.cgi_extension) && access(path.c_str(), F_OK) == 0){
            pathType = SCRIPT;
            std::string prefix("");
            resInfo.cgiBodyPath = path;
            resInfo.cgiBodyPath.append(Utils::randomName(prefix));
            return ;
        }
        if (contentType.empty() || contentType == Utils::getFileType(fileTypes, Utils::getFileName(path))){
            return;
        }
        else{
            if (!Utils::isScript(path, resInfo.location.cgi_extension))
                path.append(Utils::findExtensionByMime(fileTypes, contentType));
        }
    }
    else if (pathType == DIR_LS){
        std::string prefix("File");
        path.append("/").append(Utils::randomName(prefix)).append(Utils::findExtensionByMime(fileTypes, contentType));
    }
}


HttpStatusCode     UploadHandler::getUploadPath(std::string& uploadPath){
    HttpStatusCode status;

    uploadPath.append(resInfo.location.root);
    uploadPath.append(resInfo.location.path);
    uploadPath.append(resInfo.location.upload_store);
    if (access(uploadPath.c_str(), F_OK) != 0)
        return NOT_FOUND;
    uploadPath.append(resInfo.path.begin() + resInfo.location.root.length() + resInfo.location.path.length(), resInfo.path.end());
    status = getPathType(uploadPath, resInfo.type);
    if (status == OK)
        resInfo.path = uploadPath;
    return (status);
}

HttpStatusCode      UploadHandler::checkHeaders(std::map<std::string, std::string>& headers){
    std::map<std::string, std::string>::iterator it;
    PathTypes           pathType = DIR_LS;
    std::string         contentType;
    size_t              namePos;
    it = headers.find("content-disposition");
    if (it == headers.end() || it->second.find("form-data") == std::string::npos){
        parseState = PARSE_ERROR;
        return BAD_REQUEST;
    }
    uploadPath = std::string(resInfo.path);
    
    if ((namePos = it->second.find("filename=\"")) != std::string::npos){
        uploadPath.append("/");
        if (it->second.begin() + namePos + 10 == it->second.end() - 1)
            pathType = DIR_LS;
        else{
            uploadPath.append(it->second.begin() + namePos + 10, it->second.end() - 1);
            pathType = F;
        }
    }
    it = headers.find("content-type");
    if (it != headers.end())
        contentType = it->second;
    setFullPathByType(uploadPath, pathType, contentType);
    if (pathType == SCRIPT){
        return FORBIDDEN;
        uploadPath = resInfo.cgiBodyPath;
        pathType = F;
    }
    if (Utils::isScript(uploadPath, resInfo.location.cgi_extension)){
        return (FORBIDDEN);
    }
    resInfo.type = pathType;
    bodyFile.close();
    bodyFile.open(uploadPath.c_str(), std::ios::out | std::ios::binary);
    if (!bodyFile){
        parseState = PARSE_ERROR;
        return (INTERNAL_SERVER_ERROR);
    }
    openedFiles.push_back(uploadPath);
    return (OK);
}

HttpStatusCode  UploadHandler::extractHeaders(std::string bodyHeaders){
    std::map<std::string, std::string> headers;
    std::stringstream   ss(bodyHeaders);
    std::string         line;
    std::string         key, value;
    
    std::getline(ss, line, '\n');
    while(!ss.eof() && line != "\r"){
        if (line.find(":") == std::string::npos){
            parseState = PARSE_ERROR;
            return BAD_REQUEST;
        }
        std::stringstream headerStream(line);
        std::getline(headerStream, key, ':');
        std::getline(headerStream, value);
        if ((!Utils::isBlank(key) && key.find(' ') != std::string::npos) || value.at(value.length() - 1) != '\r')
            return (BAD_REQUEST);
        value.erase(value.length() - 1, 1);
        if (!Utils::isBlank(key) && !Utils::isBlank(value)){
            Utils::strToLower(key);
            Utils::trimSpaces(value);
            headers.insert(std::pair<std::string, std::string> (key, value));
        }
        std::getline(ss, line, '\n');
    }    
    return (checkHeaders(headers));
}


HttpStatusCode      UploadHandler::searchForBoundary(){
    long    boundPos;
    std::string start("--" + boundary + "\r\n");
    std::string end("--" + boundary + "--" + "\r\n");

    boundPos = Utils::isContainStr(&bodySaver[0], bodySaver.size(), start.c_str(), start.length());
    if (boundPos == -1){
        if (bodySaver.size() > 8000){
            parseState = PARSE_ERROR;
            return BAD_REQUEST;
        }
        return OK;
    }
    currentState = SEARCHING_HEADERS;
    if (boundPos > 0){
        if (bodyFile.is_open() && boundPos >= 2) {
            bodyFile.write(&bodySaver[0], boundPos - 2);
            bodyFile.close();
        }
    }
    bodySaver.erase(bodySaver.begin(), bodySaver.begin() + boundPos + start.length());
    if (!bodySaver.empty()){
        return searchForHeaders();
    }
    return OK;
}

HttpStatusCode      UploadHandler::searchForHeaders(){
    long            headerPos;
    HttpStatusCode  status = OK;

    headerPos = Utils::isContainStr(&bodySaver[0], bodySaver.size(), "\r\n\r\n", 4);
    if (headerPos == -1){
        return OK;
    }
    headerPos += 4;
    std::string headers(bodySaver.begin(), bodySaver.begin() + headerPos);
    status = extractHeaders(headers);
    if (status != OK && status != CREATED){
        parseState = PARSE_ERROR;
        return status;
    }
    bodySaver.erase(bodySaver.begin(), bodySaver.begin() + headerPos);
    currentState = SEARCHING_BODY;
    if (!bodySaver.empty())
        return (searchForBody());
    return status;
}

HttpStatusCode      UploadHandler::searchForBody(){
    const std::string   start("--" + boundary + "\r\n");
    const std::string   end("--" + boundary + "--" + "\r\n");
    long                boundaryPos;
    long                endPos;


    if (!bodyFile.is_open()) {
        parseState = PARSE_ERROR;
        return INTERNAL_SERVER_ERROR;
    }
    boundaryPos = Utils::isContainStr(&bodySaver[0], bodySaver.size(), start.c_str(), start.length());
    if (boundaryPos != -1){
        if (boundaryPos >= 2){
            bodyFile.write(&bodySaver[0], boundaryPos - 2);
        }
        bodyFile.close();
        currentState = SEARCHING_BOUNDARY;
        bodySaver.erase(bodySaver.begin(), bodySaver.begin() + boundaryPos);
        if (!bodySaver.empty()){
            return (searchForBoundary());
        }
        return OK;
    }
    endPos = Utils::isContainStr(&bodySaver[0], bodySaver.size(), end.c_str(), end.length());
    if (endPos != -1){
        if (endPos >= 2){
            bodyFile.write(&bodySaver[0], bodySaver.size() - end.length() - 2);
        }
        bodyFile.close();
        currentState = END;
        parseState = PARSE_COMPLETE;
        if (Utils::getFileSize(uploadPath) == 0)
            std::remove(uploadPath.c_str());
        bodySaver.clear();
        return OK;
    }
    size_t keepSize = end.length() + 4;
    if (bodySaver.size() > keepSize){
        bodyFile.write(&bodySaver[0], bodySaver.size() - keepSize);
        bodySaver.erase(bodySaver.begin(), bodySaver.end() - keepSize);
    }
    return OK;
}

HttpStatusCode      UploadHandler::multipartHandling(const char* data, size_t size){
    HttpStatusCode status = OK;
  
    Utils::pushInVector(bodySaver, data, size);
    if (currentState == SEARCHING_BOUNDARY){
        status = searchForBoundary();
    }
    if (currentState == SEARCHING_HEADERS && status == OK){
        status = searchForHeaders();;
    }
    if (currentState == SEARCHING_BODY && status == OK){
        status = searchForBody();
    }
    return status;
}


HttpStatusCode      UploadHandler::handleByContentType(const char* data, size_t size){
    HttpStatusCode status = OK;

    if (contentType.find("multipart/form-data") != std::string::npos){
        if (boundary.empty()){
            parseState = PARSE_ERROR;
            return (BAD_REQUEST);
        }
        else
            status = multipartHandling(data, size);
    }
    else {
        if (!bodyFile.is_open()){
            setFullPathByType(resInfo.path, resInfo.type, contentType);
            if (resInfo.type == F && Utils::isScript(resInfo.path, resInfo.location.cgi_extension)){
                parseState = PARSE_ERROR;
                return (FORBIDDEN);
            }
            if (resInfo.type == SCRIPT)
                bodyFile.open(resInfo.cgiBodyPath.c_str(), std::ios::out | std::ios::binary);
            else
                bodyFile.open(resInfo.path.c_str(), std::ios::out | std::ios::binary);
        }
        if (!bodyFile){
            return (INTERNAL_SERVER_ERROR);
        }
        bodyFile.write(data, size);
    }
    return (status);
}

HttpStatusCode UploadHandler::contentLengthHandling(const char* data, size_t size){
    HttpStatusCode status = OK;

    currentTotalSize += size;
    if (currentTotalSize > (size_t)uploadSize){
        parseState = PARSE_ERROR;
        currentTotalSize = 0;
        currentState = SEARCHING_BOUNDARY;
        return CONTENT_TOO_LARGE;
    }
    status = handleByContentType(data, size);
    if (parseState == PARSE_ERROR)
        currentTotalSize = 0;
    else {
        if (currentTotalSize != (size_t) uploadSize && currentState == END){
            currentTotalSize = 0;
            currentState = SEARCHING_BOUNDARY;
            parseState = PARSE_ERROR;
            return BAD_REQUEST;
        }
        if (currentTotalSize == (size_t)uploadSize){
            currentTotalSize = 0;
            if (contentType.find("multipart/form-data") != std::string::npos && currentState != END){
                currentState = SEARCHING_BOUNDARY;
                parseState = PARSE_ERROR;
                return BAD_REQUEST;
            }
            currentState = SEARCHING_BOUNDARY;
            parseState = PARSE_COMPLETE;
            bodyFile.close();
        }
    }
    return (status);
}

ParseState     UploadHandler::singleChunk(std::vector<char>& oneChunk, size_t size){
    if (oneChunk.size() <= size)
        return PARSE_ERROR;
    if (size == 0){
        if (oneChunk.size() != 2 || oneChunk[0] != '\r' || oneChunk[1] != '\n')
            return PARSE_ERROR;
        return PARSE_COMPLETE;
    }
    bodyFile.write(&oneChunk[0], size);
    return PARSE_BODY_CHUNKED;
}

HttpStatusCode UploadHandler::chunkedBodyHandling(const char* data, size_t size){
    static long chunkSize = -1;
    static bool searchForBody = false;
    long        sizePos;
    HttpStatusCode status = OK;

    Utils::pushInVector(bodySaver, data, size);
    if (!bodyFile.is_open()){
        setFullPathByType(resInfo.path, resInfo.type, contentType);
        if (resInfo.type == F && Utils::isScript(resInfo.path, resInfo.location.cgi_extension)){
            parseState = PARSE_ERROR;
            return (FORBIDDEN);
        }
        if (resInfo.type == SCRIPT)
                bodyFile.open(resInfo.cgiBodyPath.c_str(), std::ios::out | std::ios::binary);
        else
            bodyFile.open(resInfo.path.c_str(), std::ios::out | std::ios::binary);
        if (!bodyFile){
            std::cout << "ik ik ik" << std::endl;
            return (INTERNAL_SERVER_ERROR);
        }
    }
    if (!searchForBody){
        sizePos = Utils::isContainStr(&bodySaver[0], bodySaver.size(), "\r\n", 2);
        if (sizePos != -1){
            chunkSize = Utils::hexToDec(std::string(bodySaver.begin(), bodySaver.begin() + sizePos).c_str());
            if (chunkSize == -1)
                return (BAD_REQUEST);
            bodySaver.erase(bodySaver.begin(), bodySaver.begin() + sizePos + 2);
            searchForBody = true;
        }
    }
    if (searchForBody){
        if (bodySaver.size() >= (size_t)(chunkSize + 2))
        {
            parseState = singleChunk(bodySaver, chunkSize);
            bodySaver.erase(bodySaver.begin(), bodySaver.begin() + chunkSize);
            if (bodySaver[0] != '\r' || bodySaver[1] != '\n'){
                std::cout << "gia gia gia" << std::endl;
                return BAD_REQUEST;
            }
            bodySaver.erase(bodySaver.begin(), bodySaver.begin() + 2);
            searchForBody = false;
            if (!bodySaver.empty())
                status = chunkedBodyHandling("", 0);
        }
    }
    if (parseState == PARSE_ERROR)
        status = BAD_REQUEST;
    return (status);
}


ParseState  UploadHandler::upload(const char* data, size_t size){
    HttpStatusCode& status = resInfo.status;
    if (uploadPath.empty()){
        status = getUploadPath(uploadPath);
        if (status != OK)
            return PARSE_ERROR;
        std::map<std::string, std::string>::iterator it = resInfo.headers.find("content-type");
        if (it != resInfo.headers.end()){
            contentType = it->second;
            if (contentType.find("multipart/form-data") != std::string::npos){
                size_t boundaryPos;
                if ((boundaryPos = contentType.find("boundary=")) != std::string::npos){
                    boundary = std::string(contentType.begin() + boundaryPos + 9, contentType.end());
                }
                else{
                    status = BAD_REQUEST;
                    return PARSE_ERROR;
                }
            }

        }
    }
    if (parseState == PARSE_BODY_LENGTH){
        status = contentLengthHandling(data, size);
    }
    else if (parseState == PARSE_BODY_CHUNKED){
        status = chunkedBodyHandling(data, size);
    }

    if (parseState == PARSE_COMPLETE){
        bodyFile.close();
        if (Utils::getFileSize(resInfo.path) == 0)
            std::remove(resInfo.path.c_str());
        openedFiles.clear();
        status = CREATED;
    }
    else if (parseState == PARSE_ERROR){
        bodyFile.close();
        std::cout << "ANA FORM REMOVEIG" << std::endl;
        std::cout << "I will remove the file " << resInfo.path << std::endl;
        std::cout << "status = " << resInfo.status << std::endl;
        clearFiles(openedFiles);
        openedFiles.clear();
    }
    return parseState;
}

