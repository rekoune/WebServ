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
    std::string str2;
    std::string str3;
    
    // str2.append("--abde\r\nContent-Disposition: form-data; name=\"username\";filename=\"abde\"\r\n\r\nabdellah a\r\n--abde\r\n");
    // str3.append("Content-Disposition: form-data; name=\"username\";filename=\"test\"\r\n\r\nrekoune r\r\n--abde--\r\n");
    str.append("GET / HTTP/1.1\r\nHost: abdellah\r\ncontent-length: 0\r\n\r\n");
    // str.append(Utils::toString(str2.length() + str3.length()));
    // str.append("\r\n\r\n");
    // str.append(str2);
    // str.append(str3);
    // HttpStatusCode status; 
    http.appendData(str.c_str(), str.length());
    // http.appendData(str3.c_str(), str3.length());
    std::vector<char> response;
    // int i = 0;
    // while(!http.isComplete()){
    //     if (i == (int)str.length())
    //         break;
    //     // std::cout << "+++  i == " << i  << "length = " << str.length()<< std::endl;
    //     status = http.appendData(&str[i++], 1);
    // }
    if (http.isComplete()){
        response = http.getResponse();
        std::cout << "=====================Response====================" << std::endl;
        std::cout.write(response.data(), response.size()) << std::endl;
    }
    else{
        std::cout << "the request is not complite !!!" << std::endl;
    }
    // std::cout << "status 1 = " << status << std::endl;
    std::cout << "is complete = " << http.isComplete() << std::endl;
}
