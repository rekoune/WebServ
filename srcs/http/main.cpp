# include "../../includes/RequestParser.hpp"
# include "../../includes/ResourceResolver.hpp"
# include "../../includes/HttpHandler.hpp"



int main (){
    

    //srcs/http/config.conf
    GlobaConfig config ;
    if (!parseConfig("config.conf", config)){
        return(1);
    }
    HttpHandler http(config.servers[0]);

    std::string str;
    std::vector<char> res ;
    size_t total = 0;
    size_t headSize = 0;
    int a = 0;
    str.append("GET /upload/content/videos HTTP/1.1\r\nHost: arekoune\r\n\r\n");
    
    http.appendData(&str[0], str.length());
    while(!http.isResDone()){
        res = http.getResponse();
        if (a == 0){
            a = 1;
            headSize = res.size() - 10000000;
            // std::cout.write(res.data(), headSize) << std::endl;;
        }
        total += res.size();
        // std::cout << "IS Done = " << http.isResDone() << std::endl;
        // std::cout << "========= Response ===========" << std::endl;
        std::cout.write(res.data(), res.size());
    }
    std::cout << "Response size = " << total<< std::endl;
    std::cout << "======================= req 2 =============================" << std::endl;
    total = 0;
    headSize = 0;
    a = 0;
    str.clear();
    str.append("GET /upload/content/images HTTP/1.1\r\nHost: arekoune\r\n\r\n");
    http.appendData(&str[0], str.length());
    while(!http.isResDone()){
        std::cout << "ra sa ra sa"<< std::endl;
        res = http.getResponse();
        if (a == 0){
            a = 1;
            headSize = res.size() - 10000000;
            // std::cout.write(res.data(), headSize) << std::endl;;
        }
        total += res.size();
        // std::cout << "IS Done = " << http.isResDone() << std::endl;
        // std::cout << "========= Response ===========" << std::endl;
        std::cout.write(res.data(), res.size());
    }
    std::cout << "Response size = " << total<< std::endl;
}
