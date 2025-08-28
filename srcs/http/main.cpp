# include "../../includes/Request.hpp"

int main (){
    std::string reqline ("GET /pub/WWW/TheProject.html HTTP/1.1\r\n");
    Request req(reqline);
    try{
        req.parseRequest();
    }
    catch(std::exception& ex){
        std::cout << ex.what() << std::endl;
    }
}