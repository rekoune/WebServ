# include "../../../includes/UploadHandler.hpp"

UploadHandler::UploadHandler(): uploadSize(-1){
    setFileTypes();
};
UploadHandler::~UploadHandler(){};

UploadHandler::UploadHandler(const UploadHandler& other): uploadSize(-1){
    setFileTypes();
    *this = other;
};

UploadHandler& UploadHandler::operator=(const UploadHandler& other){
    this->resInfo = other.resInfo;
    this->bodySaver = other.bodySaver;
    return (*this);
}

UploadHandler::UploadHandler(HttpResourceInfo& resInfo) : uploadSize(-1){
    setFileTypes();
    this->resInfo = resInfo;
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


HttpStatusCode     UploadHandler::getUploadPath(std::string& uploadPath){
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

HttpStatusCode      UploadHandler::checkHeaders(std::map<std::string, std::string>& headers){
    std::map<std::string, std::string>::iterator it;
    PathTypes           pathType = DIR_LS;
    std::string         contentType;
    size_t              namePos;

    it = headers.find("content-disposition");
    if (it == headers.end() || it->second.find("form-data") == std::string::npos){
        std::cout << "9al 9al 9al" << std::endl;
        parseState = PARSE_ERROR;
        return BAD_REQUEST;
    }
    uploadPath = std::string(resInfo.path);
    if ((namePos = it->second.find("filename=\"")) != std::string::npos){
        uploadPath.append("/");
        uploadPath.append(it->second.begin() + namePos + 10, it->second.end() - 1);
        pathType = F;
    }
    it = headers.find("content-type");
    if (it != headers.end())
        contentType = it->second;
    setFullPathByType(uploadPath, pathType, contentType);
    bodyFile.close();
    bodyFile.open(uploadPath.c_str(), std::ios::out | std::ios::binary);
    // std::cout << "c path = " << uploadPath << std::endl;
    if (!bodyFile){
        std::cout << "lsf lsf lsf" << std::endl;
        parseState = PARSE_ERROR;
        return (INTERNAL_SERVER_ERROR);
    }
    return (OK);
}

HttpStatusCode  UploadHandler::extractHeaders(std::string bodyHeaders){
    std::map<std::string, std::string> headers;
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
    
    return (checkHeaders(headers));
}

HttpStatusCode      UploadHandler::multipartHandling(const char* data, size_t size){
    const std::string    start("--" + boundary + "\r\n");
    const std::string    end("--" + boundary + "--\r\n");
    static bool                 searchForBody = false;
    static bool                 boundaryFound = false;
    long                        boundPos;
    long                        headersPos;
    HttpStatusCode              status = OK;

    Utils::pushInVector(bodySaver, data, size);
    // std::cout << "--------------------body saver --------------" << std::endl;
    //     std::cout.write(bodySaver.data(), bodySaver.size()) << std::endl;
    // std::cout << "--------------------------------------------" << std::endl;
    boundPos = Utils::isContainStr(&bodySaver[0], bodySaver.size(), start.c_str(), start.length());
    if (boundPos != -1){
        if (Utils::isContainStr(&bodySaver[0], bodySaver.size(), end.c_str(), end.length()) == 0){
            std::cout << "end of multiparts reached !!" << std::endl;
            return (OK);
        }
        // std::cout << "ana hona test test" <<std::endl;
        boundaryFound = true;
        bodySaver.erase(bodySaver.begin(), bodySaver.begin() + boundPos + start.length());
    }
    if (!searchForBody){
        if (boundaryFound){
            headersPos = Utils::isContainStr(&bodySaver[0], bodySaver.size(), "\r\n\r\n", 4);
            if (headersPos != -1){
                status = extractHeaders(std::string(bodySaver.begin(), bodySaver.begin() + headersPos + 4));
                if (status != OK){
                    std::cout << "jf jf jf jf " << std::endl;
                    parseState = PARSE_ERROR;
                    return (BAD_REQUEST);
                }
                bodySaver.erase(bodySaver.begin(), bodySaver.begin() + headersPos + 4);
                searchForBody = true;
                boundaryFound = false;
            }
        }
    }
    if (searchForBody) {
       if (bodySaver.size() > end.length()){
        boundPos = Utils::isContainStr(&bodySaver[0], bodySaver.size(), start.c_str(), start.length());
        if (boundPos != -1){
            std::cout << "hona" << std::endl;
            bodyFile.write(&bodySaver[0], boundPos - 2);
            bodySaver.erase(bodySaver.begin(), bodySaver.begin() + boundPos);
            boundaryFound = true;
            searchForBody = false;
        }
        else {
            bodyFile.write(&bodySaver[0], bodySaver.size() - end.length());
            bodySaver.erase(bodySaver.begin(), bodySaver.begin() + (bodySaver.size() - end.length()));
        }
        if (!bodySaver.empty()){
            status = multipartHandling("", 0);
        }
       }
    }
    // std::cout << "hona status -= " << status  << std::endl;
    return status;
}

HttpStatusCode      UploadHandler::handleByContentType(const char* data, size_t size){
    HttpStatusCode status = OK;

    if (contentType.find("multipart/form-data") != std::string::npos){
        if (boundary.empty()){
            parseState = PARSE_ERROR;
            std::cout << "kaa kaa kaa boundary is empty" << std::endl;
            return (BAD_REQUEST);
        }
        else
            status = multipartHandling(data, size);
    }
    else {
        if (!bodyFile.is_open()){
            setFullPathByType(resInfo.path, resInfo.type, contentType);
            bodyFile.open(resInfo.path.c_str(), std::ios::out | std::ios::binary);
        }
        if (!bodyFile){
            std::cout << "eik eik eik" << std::endl;
            return (INTERNAL_SERVER_ERROR);
        }
        bodyFile.write(data, size);
    }
    return (status);
}

HttpStatusCode UploadHandler::contentLengthHandling(const char* data, size_t size){
    static size_t totalSize = 0;
    HttpStatusCode status = OK;

    totalSize += size;
    // if (totalSize > (size_t)uploadSize){
    //     std::cout << "sa9a sa9a sa9a" << std::endl;
    //     parseState = PARSE_ERROR;
    //     std::cout << "total size = " << totalSize << " , max size = " << uploadSize<< std::endl;
    //     exit(1);
    //     return CONTENT_TOO_LARGE;
    // }
    status = handleByContentType(data, size);
    if (totalSize == (size_t)uploadSize){
        parseState = PARSE_COMPLETE;
    }
    // if (totalSize == (size_t)uploadSize)
    return (status);
}

ParseState     UploadHandler::singleChunk(std::vector<char>& oneChunk, size_t size){
    if (oneChunk.size() <= size)
        return PARSE_ERROR;
    if (size == 0){
        if (oneChunk.size() != 2 || oneChunk[0] != '\r' || oneChunk[1] != '\n'){
            std::cout << "hey error error error " << std::endl;
            return PARSE_ERROR;
        }
        return PARSE_COMPLETE;
    }
    bodyFile.write(&oneChunk[0], size);
    return PARSE_BODY_CHUNKED;
}

HttpStatusCode UploadHandler::chunkedBodyHandling(const char* data, size_t size){
    static long chunkSize = -1;
    // static long sizeCounter = 0;
    static bool searchForBody = false;
    long        sizePos;
    HttpStatusCode status = OK;

    Utils::pushInVector(bodySaver, data, size);
    if (!bodyFile.is_open()){
        setFullPathByType(resInfo.path, resInfo.type, contentType);
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
        if (status != OK){
            return PARSE_ERROR;
        }
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

    }
    else if (parseState == PARSE_ERROR){
        bodyFile.close();
        std::remove(resInfo.path.c_str());
        std::cout << "there is an error so the file was deleted !!!" << std::endl;
    }
    return parseState;
}
