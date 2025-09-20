# include "../../includes/Request.hpp"
# include "../../includes/RequestHandler.hpp"
# include "../../includes/HttpHandler.hpp"



int main (){
    

    //srcs/http/config.conf
    GlobaConfig config ;
    if (!parseConfig("config.conf", config)){
        return(1);
    }
    std::cout << "max body size == " << config.servers[0].client_max_body_size << std::endl;
  
    HttpHandler http(config.servers[0]);

    std::string str;
    std::vector<char> response ;
    // str.append("POST /upload/Files/ HTTP/1.1\r\nHost:localhost\r\nContent-Length: 214\r\nContent-type: multipart/form-data boundary=abde\r\n\r\n");
    // str.append("--abde\r\nContent-Disposition: form-data; name=\"file1\";\r\nContent-Type: text/css\r\n\r\nmy body test--abde\r\n");
    // str.append("Content-Disposition: form-data; name=\"file1\"; filename=\"test2\"\r\nContent-Type: text/css\r\n\r\nmy body test2--abde--\r\n");
    // str.append("POST /upload/Files/test.html HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\nab");
    str.append("GET /upload/content/images/chabiba.jpg.jpeg HTTP/1.1\r\nhost: rekouneHost\r\nContent-Length: 16\r\n\r\nabdellah");
    // int i =0;
    // while(!http.isComplete()){
    //     if (i == (int)str.length())
    //         break;
    //     // std::cout << "+++  i == " << i  << "length = " << str.length()<< std::endl;
    //     http.appendData(&str[i++], 1);
    // }
    http.appendData(str.c_str(), str.length());
    if (http.isComplete() == true){
        response =  http.getResponse();
        std::cout << "+++++ The Request 1 :  " << std::endl;
        std::cout.write(response.data(), response.size());
        std::cout << std::endl;
    }
    else {
        std::cout << "+++++ NO response the request is not Complete!!" << std::endl;
    }
}
