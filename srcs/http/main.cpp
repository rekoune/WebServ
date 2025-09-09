# include "../../includes/Request.hpp"
# include "../../includes/RequestHandler.hpp"
# include "../../includes/HttpHandler.hpp"

void printStatus(HttpStatusCode status){
    std::cout << "status : ";
    switch (status)
    {
    case CONTINUE:
        std::cout << "CONTINUE" << std::endl;
        break;
    case OK:
        std::cout << "OK" << std::endl;
        break;
    case CREATED:
        std::cout << "CREATED" << std::endl;
        break;
    case NO_CONTENT:
        std::cout << "NO_CONTENT" << std::endl;
        break;
    case BAD_REQUEST:
        std::cout << "BAD_REQUEST" << std::endl;
        break;
    case UNAUTHORIZED:
        std::cout << "UNAUTHORIZED" << std::endl;
        break;
    case FORBIDDEN:
        std::cout << "FORBIDDEN" << std::endl;
        break;
    case NOT_FOUND:
        std::cout << "NOT_FOUND" << std::endl;
        break;
    case METHOD_NOT_ALLOWED:
        std::cout << "METHOD_NOT_ALLOWED" << std::endl;
        break;
    case CONTENT_TOO_LARGE:
        std::cout << "CONTENT_TOO_LARGE" << std::endl;
        break;
    case INTERNAL_SERVER_ERROR:
        std::cout << "INTERNAL_SERVER_ERROR" << std::endl;
        break;
    case NOT_IMPLEMENTED:
        std::cout << "NOT_IMPLEMENTED" << std::endl;
        break;
    case HTTP_VERSION_NOT_SUPPORTED:
        std::cout << "HTTP_VERSION_NOT_SUPPORTED" << std::endl;
        break;
    }
}


void printFileType(PathTypes type){
    std::cout << "type : ";
    switch (type)
    {
    case F:
        std::cout << "FILE" << std::endl;
        break;
    case SCRIPT:
        std::cout << "SCRIPT" << std::endl;
        break;
    case DIR_LS:
        std::cout << "DIR_LS" << std::endl;
        break;
    }
}

int main (){
    std::vector<char> vecReq;
    std::string reqline ("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 30\r\n\r\n");
    for(size_t i = 0; i < reqline.length(); i++){
        vecReq.push_back(reqline[i]);
    }
    vecReq.push_back('t');
    vecReq.push_back('e');
    vecReq.push_back('s');
    vecReq.push_back('t');
    vecReq.push_back('\0');
    vecReq.push_back('a');
    vecReq.push_back('b');
    vecReq.push_back('\0');
    vecReq.push_back('c');

    GlobaConfig config = parseConfig("config.conf");
    HttpHandler http(vecReq, config.servers[0]);

    http.handel();


    for(size_t i =0; i < vecReq.size(); i++){
        std::cout << vecReq[i] ;
    }
    std::cout << std::endl;
    std::vector<char> response =  http.getResponse();
    for(size_t i =0; i < response.size(); i++){
        std::cout << response[i] ;
    }
    
}