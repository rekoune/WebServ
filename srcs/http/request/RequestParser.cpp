# include "../../../includes/RequestParser.hpp"

RequestParser::RequestParser(){
    std::srand(std::time(NULL));
    parseState = PARSE_START;
}
RequestParser::RequestParser(const ServerConfig& server){   
    std::srand(std::time(NULL));
    parseState = PARSE_START;
    this->server = server;
    setClientMaxBody(server.client_max_body_size);
}
RequestParser::~RequestParser(){}
RequestParser::RequestParser(const RequestParser& other){
    std::srand(std::time(NULL));
    *this = other;
}
RequestParser& RequestParser::operator=(const RequestParser& other){
    // this->req = other.req;
    this->requestLine = other.requestLine;
    this->headers = other.headers;
    this->body = other.body;
    this->parseState = other.parseState;
    this->bodyMaxSize = other.bodyMaxSize;
    this->parseState = other.parseState;
    this->server = other.server;
    this->resInfo = other.resInfo;
    this->uploadHandler = other.uploadHandler;
    // this->chunkBody = other.chunkBody;
    this->clientMaxBodySize = other.clientMaxBodySize;
    return (*this);
}

void RequestParser::setClientMaxBody(size_t clientMaxBodySize){
    this->clientMaxBodySize = clientMaxBodySize;
}

RequestLine RequestParser::getRequestLine() const{
    return (this->requestLine);
}
std::map<std::string, std::string>  RequestParser::getHeaders() const{
    return (this->headers);
}
HttpResourceInfo                    RequestParser::getResourceInfo()const{
    return(this->resInfo);
}

HttpStatusCode RequestParser::setMethod(std::string& method){
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

HttpStatusCode RequestParser::setTarget(std::string& target){
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

HttpStatusCode RequestParser::setHttpVersion(std::string& httpVersion){

    if (httpVersion != "HTTP/1.1\r")
        return (HTTP_VERSION_NOT_SUPPORTED);
    else
        this->requestLine.httpVersion = httpVersion;
    return (OK);
}

HttpStatusCode RequestParser::parseRequestLine(std::string& reqLine){
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


ParseState  RequestParser::checkPostHeaders(HttpStatusCode& status){
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
            std:: cout << "hona boyd max size = " << bodyMaxSize << " , client size = " << clientMaxBodySize << std::endl;
            return (PARSE_ERROR);
        }
        return (PARSE_BODY_LENGTH);
    }
    return (PARSE_BODY_CHUNKED);
}

HttpStatusCode RequestParser::parseRequestHeaders( std::string& req){
    std::string line;
    std::string key, value;
    HttpStatusCode status;
    std::stringstream reqStream(req);

    if (reqStream.eof())
        return (BAD_REQUEST);
    getline(reqStream, line, '\n');
    while(!reqStream.eof() && line != "\r"){
        if (line.empty())
            return (BAD_REQUEST);
        std::stringstream headerStream(line);
        getline(headerStream, key, ':');
        getline(headerStream, value);
        if ((!Utils::isBlank(key) && key.find(' ') != std::string::npos) || value.at(value.length() - 1) != '\r'){
            std::cout << "ma7 ma7 ma7" << std::endl;
            return (BAD_REQUEST);
        }
        value.erase(value.length() - 1, 1);
        if (!Utils::isBlank(key) && !Utils::isBlank(value)){
            Utils::strToLower(key);
            Utils::trimSpaces(value);
            this->headers.insert(std::pair<std::string, std::string> (key, value));
        }
        getline(reqStream, line, '\n');
    }
    if (reqStream.eof() || headers.empty() || headers.find("host") == headers.end()){
        std::cout << "ta7 ta7 ta7" << std::endl;
        return (BAD_REQUEST);
    }
    else if (this->requestLine.method == "POST"){
        parseState = checkPostHeaders(status);
        if (parseState == PARSE_ERROR){
            return (status);
        }
    }
    return (OK);
}

HttpStatusCode RequestParser::parseRequest(std::string req){
    std::stringstream reqStream(req);
    std::string reqLine;
    HttpStatusCode httpStatus;

    if (req.empty())
        return (BAD_REQUEST);    
    getline(reqStream, reqLine, '\n');
    if ((httpStatus = parseRequestLine(reqLine)) == OK){
        long pos = Utils::isContainStr(req.c_str(), req.length(), "\r\n", 2) + 2;
        std::string temp(reqStream.str());
        std::string headers(temp.begin() + pos, temp.end());
        httpStatus = parseRequestHeaders(headers);
    }
    return (httpStatus);
}

bool    RequestParser::isComplete(){
    if (parseState == PARSE_COMPLETE || parseState == PARSE_ERROR){
        return (true);
    }
    return (false);
}

HttpStatusCode      RequestParser::appendData(const char* _data, size_t size){
    long            headerEndPos;
    HttpStatusCode  status = OK;

    if (parseState == PARSE_START){
        Utils::pushInVector(body, _data, size);
        headerEndPos = Utils::isContainStr(&body[0], body.size(), "\r\n\r\n", 4);
        if (headerEndPos != -1){
            headerEndPos += 4;
            status = parseRequest(std::string(body.begin(), body.begin() + headerEndPos));
            if (status != OK){
                std::cout << "bla bla bla" << std::endl;
                parseState = PARSE_ERROR;
                return status;
            }
            resourceResolver.setLocations(server.locations);
            resourceResolver.setServer(server);
            resourceResolver.setRequestLine(requestLine);
            if ((resInfo = resourceResolver.handle()).status != OK){
                // std::cout << "hora hora hora status = " << resInfo.status << std::endl;
                parseState = PARSE_ERROR;
                return resInfo.status;
            }
            resInfo.headers = headers;
            if (requestLine.method == "GET" || requestLine.method == "DELETE")
                  parseState = PARSE_COMPLETE;
            else{
                uploadHandler.setResInfo(resInfo);
                uploadHandler.setParseState(parseState);
                uploadHandler.setUploadSize(bodyMaxSize);
                parseState = uploadHandler.upload(&body[headerEndPos], body.size() - (size_t)headerEndPos);
                this -> resInfo = uploadHandler.getResourseInfo() ;
            }
        }
    }
    else{
        parseState = uploadHandler.upload(_data, size);
        this -> resInfo = uploadHandler.getResourseInfo() ;
    }
    return resInfo.status;
}