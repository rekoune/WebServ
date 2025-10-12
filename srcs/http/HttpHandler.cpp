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
    return (*this);
}

// void HttpHandler::setRequest(const Request& req){
//     this->req = req;
//     this->req.setClientMaxBody(server.client_max_body_size);
// }
void HttpHandler::setServer(const ServerConfig& server){
    this->server = server;
    this->reqParser.setClientMaxBody(server.client_max_body_size);
    this->reqParser = RequestParser(server);
}
// void HttpHandler::setResourceResolver(const ResourceResolver& reqHandler){
//     this->reqHandler = reqHandler;
// }

// void HttpHandler::handle(){
//     HttpResourceInfo resInfo;
//     if ((resInfo.status  = this->req.parseRequest()) == OK){
//         this->reqHandler = ResourceResolver(this->req, this->server);
//         resInfo = this->reqHandler.handle();
//     }
//     this->resInfo = resInfo;
// }

void HttpHandler::appendData(const char* data, size_t size){
    if (sameReq ==  false){
        // std::vector<char> test;
        // Utils::pushInVector(test, data, size);
        std::cout << "=================== new request +++++++++++++++++++++++++++++" << std::endl;
        // //std::cout.write(&test[0], test.size())<< std::endl;
        // //std::cout << "size = " <<test.size() << std::endl;
        // //std::cout << "==============================================================" << std::endl;
        this->reqParser = RequestParser(server);
        this->response = Response();
        std::cout << "kdkdkdk " << response.isDone() << std::endl;
        this->reqParser.setClientMaxBody(server.client_max_body_size);
        // this->response.clear();
        sameReq = true;
        
    }
    if (!isComplete()){
    // std::cout << "---------------------------------------------hona---------------------------------" << std::endl;
    this->resInfo.status = this->reqParser.appendData(data, size);
    if (isComplete())
        std::cout << "the Request is complete " << std::endl;
        sameReq = false;
    }
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
std::vector<char> HttpHandler::getResponse(){
    // std::cout << response.isDone() << std::endl;
    static bool test = true;
        // sameReq = false;
    // std::cout << "test = " << test << std::endl;
    if (test){
    // if (sameReq == true){
        std::cout << "BAL BAL BAL BAL BAL " << std::endl;
        test = false;
        this->response.setResInfo(reqParser.getResourceInfo());
        response.handle();
    }

    std::cout << " >> << >> is done =  = " << response.isDone() << std::endl;
    std::vector <char> res = response.getResponse();
    if (response.isDone()){
        test = true;
        std::cout << "**************** hona **********************************" << std::endl;
        std::cout << "response size = " << res.size() << std::endl;
    }
    return (res);
}