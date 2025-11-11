# include "../../includes/HttpHandler.hpp"

HttpHandler::HttpHandler():sameReq(true), sameRes(true){};
HttpHandler::~HttpHandler(){};

HttpHandler::HttpHandler (const ServerConfig& server): sameReq(true), sameRes(true){
    this->server = server;
    this->reqParser = RequestParser(server);
    this->reqParser.setClientMaxBody(server.client_max_body_size);
}
HttpHandler::HttpHandler(const HttpHandler& other){
    *this = other;
}

HttpHandler& HttpHandler::operator=(const HttpHandler& other){
    this->reqParser = other.reqParser;
    this->server = other.server;
    this->reqHandler = other.reqHandler;
    this->resInfo = other.resInfo;
    this->response = other.response;
    this->sameReq = other.sameReq;
    this->sameRes = other.sameRes;
    return (*this);
}

void HttpHandler::setServer(const ServerConfig& server){
    this->server = server;
    this->reqParser.setClientMaxBody(server.client_max_body_size);
    this->reqParser = RequestParser(server);
}

bool HttpHandler::isComplete(){
    return (this->reqParser.isComplete());
}
bool HttpHandler::isKeepAlive(){
    return response.isKeepAlive();
}
bool HttpHandler::isResDone(){
    return response.isDone();
}
int HttpHandler::isScript(){
    return (this->reqParser.getResourceInfo().cgiFD);
}

std::vector<char> HttpHandler::getStatusResponse(const HttpStatusCode& statusCode){
    return (this->response.getStatusResponse(statusCode));
}

void HttpHandler::appendData(const char* data, size_t size){
    if (sameReq ==  false){
        this->reqParser = RequestParser(server);
        this->response = Response();
        this->reqParser.setClientMaxBody(server.client_max_body_size);
        sameReq = true;
    }
    if (!isComplete()){
        this->resInfo.status = this->reqParser.appendData(data, size);
    if (isComplete())
        sameReq = false;
    }
}

std::vector<char> HttpHandler::getResponse(){
    if (sameRes){
        sameRes = false;
        this->response.setResInfo(reqParser.getResourceInfo());
        this->response.setCgiExecutor(reqParser.getCgiExecutor());
        response.handle();
    }

    std::vector <char> res;
    if (!response.isDone()){
        res = response.getResponse();
    }
    if (response.isDone()){
        sameRes = true;
    }
    return (res);
}
