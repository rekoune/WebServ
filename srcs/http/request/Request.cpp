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
    this->method = other.method;
    this->target = other.target;
    this->HttpVersion = other.HttpVersion;
    return (*this);
}

void Request::parseRequest(){
    std::stringstream reqStream(this->req);
    std::string reqLine;

    getline(reqStream, reqLine, '\n');
    parseRequestLine(reqLine);
}
void Request::setMethod(std::string& method){
    std::cout << "methos = " << method<< std::endl;
    if (method == "GET")
        this->method = GET;
    else if (method == "POST")
        this->method = POST;
    else if (method == "DELETE")
        this->method = DELETE;
    else{
        std::cout << "in set Method" << std::endl;
        throw BadRequest();
    }
}

void Request::setTarget(std::string& target){
    std::cout << "target = " << target<< std::endl;
}
void Request::setHttpVersion(std::string& httpVersion){
    if (httpVersion.length() != 9){
        std::cout << "in set http version" << std::endl;
        throw BadRequest();
    }
    
}

void Request::parseRequestLine(std::string& reqLine){
    if (reqLine.at(reqLine.length() - 1) != '\r'){
        std::cout << "in set parseRequestLine" << std::endl;
        throw BadRequest();
    }
    std::string line;
    std::stringstream lineStream(reqLine);
    getline(lineStream, line, ' ');
    setMethod(line);
    getline(lineStream, line, ' ');
    setTarget(line);
    getline(lineStream, line, ' ');
    setHttpVersion(line);

    if (!lineStream.eof()){
        std::cout << "in set parseRequestLine" << std::endl;
        throw BadRequest();
    }

}

const char* Request::BadRequest::what() const throw(){
    return ("bad request");
}