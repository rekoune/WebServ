# include "../../includes/Request.hpp"

int main (){
    std::vector<char> vecReq;
    std::string reqline ("GET /home HTTP/1.1\r\nabde: test\r\nrekoune: abdellah\r\n\r\n");
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
    Request req(vecReq);
    HttpStatusCode status = req.parseRequest();
    std::cout << "status = " << status << std::endl;
    // std::string line;
    // std::stringstream test;
    // test << "abdellah";

    // getline(test, line);
    // std::cout << "line = " << line << std::endl;
    // std::cout << "test = " << test.str() << std::endl;
}