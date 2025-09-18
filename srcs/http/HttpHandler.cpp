# include "../../includes/HttpHandler.hpp"

HttpHandler::HttpHandler():sameReq(true){};
HttpHandler::~HttpHandler(){};

// HttpHandler::HttpHandler (std::vector<char> req, const ServerConfig& server){
//     this->req = Request(req);
//     this->server = server;
//     this->req.setClientMaxBody(server.client_max_body_size);
// }
HttpHandler::HttpHandler (const ServerConfig& server): sameReq(true){
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
    this->sameReq = other.sameReq;
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

// void HttpHandler::handle(){
//     HttpResponseInfo resInfo;
//     if ((resInfo.status  = this->req.parseRequest()) == OK){
//         this->reqHandler = RequestHandler(this->req, this->server);
//         resInfo = this->reqHandler.handle();
//     }
//     this->resInfo = resInfo;
// }

void HttpHandler::appendData(const char* data, size_t size){
    if (sameReq ==  false){
        this->req = Request();
        this->req.setClientMaxBody(server.client_max_body_size);
        this->response.clear();
        sameReq = true;
    }
    this->resInfo.status = this->req.appendData(data, size);
}

bool HttpHandler::isComplete(){
    return (this->req.isComplete());
}

std::vector<char> HttpHandler::getResponse(){

    if (this->resInfo.status == OK){
        this->reqHandler.setRequest(this->req);
        this->reqHandler.setServer(this->server);
        this->resInfo = this->reqHandler.handle();
    }
    
    this->response.setResInfo(this->resInfo);
    response.handle();
    sameReq = false;
    // std::cout << "status = " << resInfo.status << std::endl;
    return (response.getResponse());
}