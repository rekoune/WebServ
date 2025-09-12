# include "../../includes/Request.hpp"
# include "../../includes/RequestHandler.hpp"
# include "../../includes/HttpHandler.hpp"



int main (){
    

    GlobaConfig config = parseConfig("config.conf");
  
    HttpHandler http;

    std::string str("POST / HTTP/1.1\r\nHost: localhost\r");

    http.setServer(config.servers[0]);

    http.appendData(str.c_str(), str.length());
    str = "\nTransfer-Encoding: chunked\r\n";
    http.appendData(str.c_str(), str.length());

    std::cout << "is complete = " << http.isComplete() << std::endl;
    std::vector<char> response =  http.getResponse();
    std::cout << "==================== Response ===================" << std::endl;
    for(size_t i =0; i < response.size(); i++){
        std::cout << response[i] ;
    }

    
}


