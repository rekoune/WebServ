# include "../../includes/Request.hpp"
# include "../../includes/RequestHandler.hpp"
# include "../../includes/HttpHandler.hpp"



int main (){
    

    GlobaConfig config = parseConfig("config.conf");
  
    HttpHandler http(config.servers[0]);

    std::string str;
    str.append("POST / HTTP/1.1\r\nHost: localhost\r\nTraNsfer-Encoding: chunked\r\n\r\n0\r\n\r\n");
    // str.append("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\n2a");

    int i =0;
    while(!http.isComplete()){
        if (i == (int)str.length())
            break;
        // std::cout << "+++  i == " << i  << "length = " << str.length()<< std::endl;
        http.appendData(&str[i++], 1);
    }
    // http.appendData(str.c_str(), str.length());
    if (http.isComplete() == true){
        std::vector<char> response =  http.getResponse();
        std::cout << "+++++ The Request is completed, Response :  " << std::endl;
        for(size_t i =0; i < response.size(); i++){
            std::cout << response[i] ;
        }
    }
    else {
        std::cout << "+++++ NO response the request is not Complete!!" << std::endl;
    }

    
}



