# include "../../../includes/Request.hpp"

Request::Request(){
    std::srand(std::time(NULL));
    parseState = PARSE_START;
}
Request::~Request(){}
Request::Request(const Request& other){
    std::srand(std::time(NULL));
    *this = other;
}
// Request::Request(std::vector<char> req){
//     // this->bodyIndex = splitHttpRequest(req);
//     parseState = PARSE_START;
//     this->body = req;
// }

// void Request::setRequest(std::vector<char> req){
//     this->bodyIndex = splitHttpRequest(req);
//     this->body = req;
// }
void Request::setClientMaxBody(size_t clientMaxBodySize){
    this->clientMaxBodySize = clientMaxBodySize;
}

Request& Request::operator=(const Request& other){
    this->req = other.req;
    this->requestLine = other.requestLine;
    this->headers = other.headers;
    this->body = other.body;
    this->parseState = other.parseState;
    this->bodyMaxSize = other.bodyMaxSize;
    this->chunkBody = other.chunkBody;
    this->clientMaxBodySize = other.clientMaxBodySize;
    return (*this);
}


RequestLine Request::getRequestLine() const{
    return (this->requestLine);
}
std::map<std::string, std::string>  Request::getHeaders() const{
    return (this->headers);
}
std::vector<char>   Request::getBody() const{
    return (this->body);
}

HttpStatusCode Request::setMethod(std::string& method){
    if (method == "GET")
        this->requestLine.method = "GET";
    else if (method == "POST")
        this->requestLine.method = "POST";
    else if (method == "DELETE")
        this->requestLine.method = "DELETE";
    else
        return (BAD_REQUEST);
    return (OK);
}

HttpStatusCode Request::setTarget(std::string& target){
    std::deque<std::string> deque;
    std::stringstream targetStream(target);
    std::string path;

    if (target.empty() || target.at(0) != '/')
        return (BAD_REQUEST);
    while(!targetStream.eof()){
        std::getline(targetStream, path, '/');
        if (!path.empty() && path != "."){
            if (path == ".."){
                if (!deque.empty())
                    deque.pop_back();
            }
            else
                deque.push_back(path);
        }
    }
    this->requestLine.target.append("/");
    while(deque.size()){
        this->requestLine.target.append(deque.front());
        deque.pop_front();
        if (deque.size())
            this->requestLine.target.append("/");
    }
    if (target.length() > 1 && target.at(target.length() - 1) == '/' && target.at(target.length() - 2) != '/' )
        this->requestLine.target.append("/");
    return (OK);
}

HttpStatusCode Request::setHttpVersion(std::string& httpVersion){

    if (httpVersion != "HTTP/1.1\r")
        return (HTTP_VERSION_NOT_SUPPORTED);
    else
        this->requestLine.httpVersion = httpVersion;
    return (OK);
}

HttpStatusCode Request::parseRequestLine(std::string& reqLine){
    if (reqLine.at(reqLine.length() - 1) != '\r')
        return (BAD_REQUEST);
    std::string line;
    std::stringstream lineStream(reqLine);
    HttpStatusCode statusCode;

    getline(lineStream, line, ' ');
    if ((statusCode = setMethod(line)) == OK){
        getline(lineStream, line, ' ');
        if ((statusCode = setTarget(line)) == OK){
            getline(lineStream, line, ' ');
            if ((statusCode = setHttpVersion(line)) != OK)
                return (statusCode);
        }
        else
            return (statusCode);
    }
    else 
        return (statusCode);

    if (!lineStream.eof())
        return (BAD_REQUEST);

    return (OK);

}

ParseState  Request::checkPostHeaders(HttpStatusCode& status){
    std::map<std::string, std::string>::iterator length, chunked;

    length = headers.find("content-length");
    chunked = headers.find("transfer-encoding");

    
    if (length == headers.end() && chunked == headers.end()){
        status = BAD_REQUEST;
        return (PARSE_ERROR);
    }
    else if (length != headers.end() && chunked != headers.end() ){
        status = BAD_REQUEST;
        return (PARSE_ERROR);
    }
    if (length != headers.end()){
        std::stringstream ss(length->second);
        ss >> bodyMaxSize;
        if (bodyMaxSize < 0){
            status = BAD_REQUEST;
            return (PARSE_ERROR);
        }
        else if (bodyMaxSize > (long)clientMaxBodySize){
            status = CONTENT_TOO_LARGE;
            return (PARSE_ERROR);
        }
        return (PARSE_BODY_LENGTH);
    }
    return (PARSE_BODY_CHUNKED);
}

HttpStatusCode Request::parseRequestHeaders( std::stringstream& req){
    std::string line;
    std::string key, value;
    HttpStatusCode status;

    if (req.eof())
        return (BAD_REQUEST);
    getline(req, line, '\n');
    while(!req.eof() && line != "\r"){
        if (line.empty())
            return (BAD_REQUEST);
        std::stringstream headerStream(line);
        getline(headerStream, key, ':');
        getline(headerStream, value);
        if ((!Utils::isBlank(key) && key.find(' ') != std::string::npos) || value.at(value.length() - 1) != '\r')
            return (BAD_REQUEST);
        value.erase(value.length() - 1, 1);
        if (!Utils::isBlank(key) && !Utils::isBlank(value)){
            Utils::strToLower(key);
            Utils::trimSpaces(value);
            this->headers.insert(std::pair<std::string, std::string> (key, value));
        }
        getline(req, line, '\n');
    }
    if (req.eof() || headers.empty() || headers.find("host") == headers.end())
        return (BAD_REQUEST);
    else if (this->requestLine.method == "POST"){
        parseState = checkPostHeaders(status);
        if (parseState == PARSE_ERROR){
            return (status);
        }
    }
    return (OK);
}

// HttpStatusCode Request::parseBody(){
//     this->body.erase(this->body.begin(), this->body.begin() + bodyIndex);
//     return OK;
// }
//delete
void Request::printHeaders(){
    std::map<std::string, std::string>::iterator it = this->headers.begin();
    while(it != this->headers.end()){
        std::cout << "H > " << it->first << ":" << it->second << std::endl;
        it++;
    }
}
void Request::printBody(){
    std::cout << "body size : " << body.size() << std::endl;
    std::cout << "body : |";
    for(size_t i = 0; i < body.size(); i++){
        std::cout << body[i];
    }
    std::cout << "| end" << std::endl;
    write(1, "\n", 1);
}

// int    Request::splitHttpRequest(std::vector<char>& req){
//     size_t i = 0;

//     while(i < req.size()){
//         if (req.at(i) == '\r' && i + 3 < req.size()){
//             if (req.at(i + 1) == '\n' && req.at(i + 2) == '\r' && req.at(i + 3) == '\n'){
//                 this->req.append("\r\n\r\n");
//                 return (i + 4);
//             }
//         }
//         this->req.push_back(req.at(i));
//         i++;
//     }
//     return (i);
// } 

HttpStatusCode Request::parseRequest(){
    std::stringstream reqStream(this->req);
    std::string reqLine;
    HttpStatusCode httpStatus;

    if (this->req.empty())
        return (BAD_REQUEST);
    getline(reqStream, reqLine, '\n');
    if ((httpStatus = parseRequestLine(reqLine)) == OK){
        httpStatus = parseRequestHeaders(reqStream);
    }
    return (httpStatus);
}

bool    Request::isComplete(){
    if (parseState == PARSE_COMPLETE || parseState == PARSE_ERROR){
        return (true);
    }
    return (false);
}

// ParseState  Request::getBodyType(){
//     std::map<std::string, std::string>::iterator it;

//     if (parseState == PARSE_BODY_LENGTH){
//         if (body.size() >= (size_t)bodyMaxSize)
//             return (PARSE_COMPLETE);
//         return (PARSE_BODY_LENGTH);
//     }
//     else if (headers.find("transfer-encoding") != headers.end() && headers.find("transfer-encoding")->second == "chunked")
//         return (PARSE_BODY_CHUNKED);
//     return (PARSE_COMPLETE);
// }


// ParseState     Request::singleChunk(std::vector<char> oneChunk, long sizePos, long bodyPos){
//     long    size;
//     std::vector<char> temp;
//     std::vector<char> tempBody;
// std::cout << "*******************single chunk body********************" << std::endl;
// std::cout.write(oneChunk.data(), oneChunk.size());
// std::cout << "*******************************************" << std::endl;
//     Utils::pushInVector(temp, &oneChunk[0], sizePos);
//     temp.push_back('\0');
//     size = Utils::hexToDec(&temp[0]);
//     if (size == 0)
//         return (PARSE_COMPLETE);
//     sizePos += 2;
//     Utils::pushInVector (tempBody, &oneChunk[sizePos], bodyPos - sizePos);
//     if (size != (long)tempBody.size()){
//         //hona hona hona
//         return PARSE_ERROR;
//     }
//     Utils::pushInVector (body, &tempBody[0], tempBody.size());
//     return (PARSE_BODY_CHUNKED);
// }










ParseState     Request::singleChunk(std::vector<char> oneChunk, size_t size){
    if (oneChunk.size() <= size)
        return PARSE_ERROR;
    if (size == 0){
        if (oneChunk.size() != 2 || oneChunk[0] != '\r' || oneChunk[1] != '\n'){
            std::cout << "hey error error error " << std::endl;
            return PARSE_ERROR;
        }
        return PARSE_COMPLETE;
    }
    Utils::pushInVector(body, &oneChunk[0], size);
    return PARSE_BODY_CHUNKED;
}

ParseState  Request::unchunk(){
    long    currentPos = 0;
    long    sizePos = 0;
    long    size;
    ParseState status;
    std::vector<char> tempSize;

    sizePos = Utils::isContainStr(&chunkBody[0], chunkBody.size(), "\r\n", 2);
    if (sizePos == -1)
        return PARSE_ERROR;
    Utils::pushInVector(tempSize, &chunkBody[0], sizePos);
    tempSize.push_back('\0');
    size = Utils::hexToDec(&tempSize[0]);
    if (size == -1){
        std::cout << "hona hona hona" << std::endl;
        return PARSE_ERROR;
    }
    else if (size == 0)
        return (PARSE_COMPLETE);
    currentPos += sizePos + 2;
    std::vector<char> tempChunk(chunkBody.begin() + currentPos, chunkBody.end());
    while((status = singleChunk(tempChunk, size)) == PARSE_BODY_CHUNKED){
        currentPos += size;
        if (chunkBody[currentPos] != '\r' || chunkBody[currentPos + 1] != '\n'){
            std::cout << "zika zika zika" << std::endl;
            return PARSE_ERROR;
        }
        currentPos += 2;
        sizePos = Utils::isContainStr(&chunkBody[currentPos], chunkBody.size() - currentPos, "\r\n", 2);
        tempSize.clear();
        Utils::pushInVector(tempSize, &chunkBody[currentPos], sizePos);
        tempSize.push_back('\0');
        size = Utils::hexToDec(&tempSize[0]);
        currentPos += sizePos + 2;
        tempChunk.clear();
        Utils::pushInVector(tempChunk, &chunkBody[currentPos], chunkBody.size() - currentPos);
    }
    return status;
}











// ParseState  Request::unchunk(){
//     long sizePos = 0l;
//     long bodyPos = 0l;
//     std::vector<char> tempChunk;
//     long currentPos = 0;
//     std::cout << "------------------the full  chunked body------------------" << std::endl;
//     std::cout.write(chunkBody.data(), chunkBody.size());
//     std::cout << "----------------------------------------" << std::endl;

//     while(currentPos < (long)chunkBody.size() && (parseState == PARSE_BODY_CHUNKED)){
//         sizePos = Utils::isContainStr(&chunkBody[currentPos], chunkBody.size() - currentPos, "\r\n", 2);
//         bodyPos = Utils::isContainStr(&chunkBody[currentPos + sizePos + 2], chunkBody.size() - currentPos - sizePos - 2, "\r\n", 2) + 2 + sizePos;
//         tempChunk.clear();
//         Utils::pushInVector(tempChunk, &chunkBody[currentPos], (bodyPos + 2));
//         currentPos += bodyPos + 2;
//         parseState = singleChunk(tempChunk, sizePos, bodyPos);
//     }
//     if (parseState != PARSE_ERROR){
//         if (chunkBody.size() > 5)
//             bodyPos = chunkBody.size() - 5;
//         else
//             bodyPos = chunkBody.size();
//         sizePos = Utils::isContainStr(&chunkBody[chunkBody.size() - 5], bodyPos, "0\r\n\r\n", 5);
//         if (sizePos == -1){
//             parseState = PARSE_ERROR;
//         std::cout << "ya3 ya3 ya3" << std::endl;
//         }
//         else 
//             parseState = PARSE_COMPLETE;
//     }
//     // std::cout << "body size = " << body.size() << std::endl;
//     return (parseState);
// }

void    Request::appendBody(const char* _data, size_t size, HttpStatusCode& status){
    if (parseState == PARSE_BODY_LENGTH){
        Utils::pushInVector(this->body, _data, size);
        if (this->body.size() >= (size_t)bodyMaxSize){
            parseState = PARSE_COMPLETE;
        }
    }

    else if (parseState == PARSE_BODY_CHUNKED){
        Utils::pushInVector(chunkBody, _data, size);
        if (Utils::isContainStr(&chunkBody[0], chunkBody.size(), "0\r\n\r\n", 5) != -1){
            parseState = unchunk();
        }
    }
    if (parseState == PARSE_COMPLETE){
        if (this->body.size() > (size_t)bodyMaxSize || 
            (!this->body.empty() && this->requestLine.method == "GET")){
            std::cout << "hora hora hora" << std::endl;
            status = BAD_REQUEST;
        }
    // printBody();
    }
    else if (parseState == PARSE_ERROR){
        std::cout << "billa billa billa" << std::endl;
        status = BAD_REQUEST;
    }

}

HttpStatusCode    Request::appendData(const char* _data, size_t size){
    long pos;
    HttpStatusCode status = OK;
    std::vector<char> tempBuffer;

    if (parseState == PARSE_START){
        Utils::pushInVector(chunkBody, _data, size);
        pos = Utils::isContainStr(&chunkBody[0], chunkBody.size(), "\r\n\r\n", 4);
        if (pos != -1){
            tempBuffer = chunkBody;
            chunkBody.clear();
            this->req.append(tempBuffer.begin(), tempBuffer.begin() + pos + 4);
            status = parseRequest();
            // printHeaders();
            if (status != OK){
                std::cout << "bla bla bla" << std::endl;
                parseState = PARSE_ERROR;
                return (status);
            }
            if (requestLine.method == "GET" || requestLine.method == "DELETE")
                  parseState = PARSE_COMPLETE;
            if (parseState == PARSE_BODY_LENGTH){
                Utils::pushInVector(this->body, &tempBuffer[pos + 4], tempBuffer.size() - pos - 4);
                if (this->body.size() >= (size_t)bodyMaxSize)
                    parseState = PARSE_COMPLETE;
            }
            else if (parseState == PARSE_BODY_CHUNKED){
                Utils::pushInVector(chunkBody, &tempBuffer[pos + 4], tempBuffer.size() - pos - 4);
                if (Utils::isContainStr(&chunkBody[0], chunkBody.size(), "0\r\n\r\n", 5) != -1){
                    std::cout << "------------- hona one complete request -----------------" << std::endl;
                    parseState = unchunk();
                }
            }
        }
    }
    else 
        appendBody(_data, size, status);
    if ((long)body.size() > bodyMaxSize || parseState == PARSE_ERROR){
        std::cout << "body size = " << body.size() << " , body max size = " << bodyMaxSize << std::endl;
        std::cout << "ma3a ma3a ma3a" << std::endl;
        return BAD_REQUEST;
    }
        // printBody();
    // printHeaders(); 
    return (status);
}
