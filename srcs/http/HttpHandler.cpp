# include "../../includes/HttpHandler.hpp"

HttpHandler::HttpHandler(){};
HttpHandler::~HttpHandler(){};

HttpHandler::HttpHandler (std::vector<char> req, const ServerConfig& server){
    this->req = Request(req);
    this->server = server;
}
HttpHandler::HttpHandler(const HttpHandler& other){
    *this = other;
}

HttpHandler& HttpHandler::operator=(const HttpHandler& other){
    this->req = other.req;
    this->server = other.server;
    this->reqHandler = other.reqHandler;
    this->resInfo = other.resInfo;
    this->response = other.response;
    return (*this);
}

void HttpHandler::setRequest(const Request& req){
    this->req = req;
}
void HttpHandler::setServer(const ServerConfig& server){
    this->server = server;
}
void HttpHandler::setRequestHandler(const RequestHandler& reqHandler){
    this->reqHandler = reqHandler;
}

void HttpHandler::handel(){
    HttpResponseInfo resInfo;
    if ((resInfo.status  = this->req.parseRequest()) == OK){
        this->reqHandler = RequestHandler(this->req, this->server);
        resInfo = this->reqHandler.handle();
    }
    this->resInfo = resInfo;
}

std::vector<char> HttpHandler::getResponse(){
    Response res(this->resInfo);

    res.handel();
    return (res.getResponse());
}