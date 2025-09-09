# include "../../../includes/Request.hpp"

Request::Request(){}
Request::~Request(){}
Request::Request(const Request& other){
    *this = other;
}
Request::Request(std::vector<char> req){
    this->bodyIndex = splitHttpRequest(req);
    this->body = req;
}

void Request::setRequest(std::vector<char> req){
    this->bodyIndex = splitHttpRequest(req);
    this->body = req;
}

Request& Request::operator=(const Request& other){
    this->req = other.req;
    this->requestLine = other.requestLine;
    this->headers = other.headers;
    this->body = other.body;
    this->bodyIndex = other.bodyIndex;
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

HttpStatusCode Request::parseRequestHeaders( std::stringstream& req){
    std::string line;
    std::string key, value;

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
        if (!Utils::isBlank(key) && !Utils::isBlank(value))
            this->headers.insert(std::pair<std::string, std::string> (key, value));
        getline(req, line, '\n');
    }
    if (req.eof() || headers.empty() || headers.find("Host") == headers.end())
        return (BAD_REQUEST);
    else if (this->requestLine.method == "POST" &&
        headers.find("Content-Length") == headers.end() &&
        headers.find("Transfer-Encoding") == headers.end())
        return (BAD_REQUEST);
    return (OK);
}

HttpStatusCode Request::parseBody(){
    this->body.erase(this->body.begin(), this->body.begin() + bodyIndex);
    return OK;
}
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
        std::cout <<body.at(i) ;
    }
    std::cout << "| end" << std::endl;
}

int    Request::splitHttpRequest(std::vector<char>& req){
    size_t i = 0;

    while(i < req.size()){
        if (req.at(i) == '\r' && i + 3 < req.size()){
            if (req.at(i + 1) == '\n' && req.at(i + 2) == '\r' && req.at(i + 3) == '\n'){
                this->req.append("\r\n\r\n");
                return (i + 4);
            }
        }
        this->req.push_back(req.at(i));
        i++;
    }
    return (i);
}

HttpStatusCode Request::parseRequest(){
    std::stringstream reqStream(this->req);
    std::string reqLine;
    HttpStatusCode httpStatus;

    if (this->req.empty())
        return (BAD_REQUEST);
    getline(reqStream, reqLine, '\n');
    if ((httpStatus = parseRequestLine(reqLine)) == OK){
        if ((httpStatus = parseRequestHeaders(reqStream)) == OK){
            // printHeaders();
            parseBody();
            // printBody();
        }
        else
            return (httpStatus);
    }
    else
        return (httpStatus);
    return (OK);
}
