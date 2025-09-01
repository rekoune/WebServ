# include "../../includes/Request.hpp"

int main (){
    std::string reqline ("GET /home HTTP/1.1\r\n hona: test\r\n abde: rekoune\r\n\r\n");
    Request req(reqline);
    HttpStatusCode status = req.parseRequest();
    std::cout << "status = " << status << std::endl;
}