# include "../../includes/HttpHandler.hpp"

HttpHandler::HttpHandler(){};
HttpHandler::~HttpHandler(){};

// HttpHandler::HttpHandler (std::vector<char> req, const ServerConfig& server){
//     this->req = Request(req);
//     this->server = server;
//     this->req.setClientMaxBody(server.client_max_body_size);
// }
HttpHandler::HttpHandler (const ServerConfig& server){
    this->server = server;
    this->req.setClientMaxBody(server.client_max_body_size);
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

// void HttpHandler::setRequest(const Request& req){
//     this->req = req;
//     this->req.setClientMaxBody(server.client_max_body_size);
// }
void HttpHandler::setServer(const ServerConfig& server){
    this->server = server;
    this->req.setClientMaxBody(server.client_max_body_size);
}
// void HttpHandler::setRequestHandler(const RequestHandler& reqHandler){
//     this->reqHandler = reqHandler;
// }

// void HttpHandler::handel(){
//     HttpResponseInfo resInfo;
//     if ((resInfo.status  = this->req.parseRequest()) == OK){
//         this->reqHandler = RequestHandler(this->req, this->server);
//         resInfo = this->reqHandler.handle();
//     }
//     this->resInfo = resInfo;
// }

void HttpHandler::appendData(const char* data, size_t size){
    if (this->resInfo.status == OK)
        this->resInfo.status = this->req.appendData(data, size);
}

bool HttpHandler::isComplete(){
    return (this->req.isComplete());
}

std::vector<char> HttpHandler::getResponse(){
    if (this->resInfo.status == OK){
        this->reqHandler = RequestHandler(this->req, this->server);
        this->resInfo = this->reqHandler.handle();
    }
    Response res(this->resInfo);
    res.handel();
    return (res.getResponse());
}