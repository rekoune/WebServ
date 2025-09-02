# include "../../includes/Request.hpp"

int main (){
    std::string reqline ("GET /home HTTP/1.1\r\ndd:    ss\r\nrekoune:  abde  llah\r\n\r\n");
    Request req(reqline);
    HttpStatusCode status = req.parseRequest();
    std::cout << "status = " << status << std::endl;
}