# include "../../../includes/Request.hpp"

Request::Request(){}
Request::~Request(){}
Request::Request(const Request& other){
    *this = other;
}
Request::Request(std::string req){
    this->req = req;
}
Request& Request::operator=(const Request& other){
    this->req = other.req;
    this->requestLine.method = other.requestLine.method;
    this->requestLine.target = other.requestLine.target;
    this->requestLine.httpVersion = other.requestLine.httpVersion;
    return (*this);
}

HttpStatusCode Request::setMethod(std::string& method){
    if (method == "GET")
        this->requestLine.method = GET;
    else if (method == "POST")
        this->requestLine.method = POST;
    else if (method == "DELETE")
        this->requestLine.method = DELETE;
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
    std::cout << "method = " << this->requestLine.method << std::endl;
    std::cout << "target = " << this->requestLine.target << std::endl;
    std::cout << "httpVersion = " << this->requestLine.httpVersion << std::endl;
    return (OK);

}

// HttpStatusCode Request::parseRequestHeaders( std::stringstream& req){

// }

HttpStatusCode Request::parseRequest(){
    std::stringstream reqStream(this->req);
    std::string reqLine;
    HttpStatusCode httpStatus;

    getline(reqStream, reqLine, '\n');
    if ((httpStatus = parseRequestLine(reqLine)) == OK){
        
    }
    else
        return (BAD_REQUEST);
    return (OK);
}
