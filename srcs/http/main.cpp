# include "../../includes/Request.hpp"
# include "../../includes/RequestHandler.hpp"
# include "../../includes/HttpHandler.hpp"



// int main (){
    


//     GlobaConfig config = parseConfig("config.conf");
  
//     HttpHandler http(config.servers[0]);

//     std::string str;
//     std::vector<char> response ;
//     str.append("POST /upload/Files/ HTTP/1.1\r\nHost:localhost\r\nContent-Length: 214\r\nContent-type: multipart/form-data boundary=abde\r\n\r\n");
//     str.append("--abde\r\nContent-Disposition: form-data; name=\"file1\";\r\nContent-Type: text/css\r\n\r\nmy body test--abde\r\n");
//     str.append("Content-Disposition: form-data; name=\"file1\"; filename=\"test2\"\r\nContent-Type: text/css\r\n\r\nmy body test2--abde--\r\n");
//     // str.append("POST /upload/Files/test.html HTTP/1.1\r\nHost: localhost\r\nContent-Length: 2\r\n\r\nab");
//     // str.append("GET / HTTP/1.1\r\nhost: rekouneHost\r\n\r\n");
//     int i =0;
//     while(!http.isComplete()){
//         if (i == (int)str.length())
//             break;
//         // std::cout << "+++  i == " << i  << "length = " << str.length()<< std::endl;
//         http.appendData(&str[i++], 1);
//     }
//     // http.appendData(str.c_str(), str.length());
    // if (http.isComplete() == true){
    //     response =  http.getResponse();
    //     std::cout << "+++++ The Request is completed, Response :  " << std::endl;
    //     for(size_t i =0; i < response.size(); i++){
    //         std::cout << response[i] ;
    //     }
    // }
//     else {
//         std::cout << "+++++ NO response the request is not Complete!!" << std::endl;
//     }
    
// }


#include "../../includes/Request.hpp"
#include "../../includes/RequestHandler.hpp"
#include "../../includes/HttpHandler.hpp"
#include <iostream>
#include <vector>
#include <string>

void test_multipart_basic() {
    std::cout << "=== Test 1: Basic Multipart Form ===\n";
    
    GlobaConfig config = parseConfig("config.conf");
    HttpHandler http(config.servers[0]);
    
    std::string request = 
        "POST /upload/Files HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: multipart/form-data; boundary=----TestBoundary\r\n"
        "Content-Length: 248\r\n"
        "\r\n"
        "------TestBoundary\r\n"
        "Content-Disposition: form-data; name=\"text_field\"\r\n"
        "\r\n"
        "This is text value\r\n"
        "------TestBoundary\r\n"
        "Content-Disposition: form-data; name=\"file\"; filename=\"test.txt\"\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "File content here\r\n"
        "------TestBoundary--\r\n";
    
    http.appendData(request.c_str(), request.size());
    
    if (http.isComplete()) {
        std::vector<char> response = http.getResponse();
        std::string resp_str(response.begin(), response.end());
        std::cout << "Response: " << resp_str.substr(9, 3) << std::endl;
    }
}

void test_multipart_no_filename() {
    std::cout << "\n=== Test 2: Multipart Without Filename ===\n";
    
    GlobaConfig config = parseConfig("config.conf");
    HttpHandler http(config.servers[0]);
    std::vector<char> response;
    std::string request = 
        "POST /upload/Files HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: multipart/form-data; boundary=----TestBoundary\r\n"
        "Content-Length: 103\r\n"
        "\r\n"
        "------TestBoundary\r\n"
        "Content-Disposition: form-data; name=\"username\"\r\n"
        "\r\n"
        "john_doe\r\n"
        "------TestBoundary--\r\n";
    
    http.appendData(request.c_str(), request.size());
    std::cout << "complete = " << http.isComplete() << std::endl;
    if (http.isComplete() == true){
        response =  http.getResponse();
        std::cout << "+++++ The Request is completed, Response :  " << std::endl;
        for(size_t i =0; i < response.size(); i++){
            std::cout << response[i] ;
        }
    }
}

void test_url_encoded_form() {
    std::cout << "\n=== Test 3: URL-encoded Form ===\n";
    
    GlobaConfig config = parseConfig("config.conf");
    HttpHandler http(config.servers[0]);
    std::vector<char> response;
    std::string request = 
        "POST /upload/Files/ HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 24\r\n"
        "\r\n"
        "name=John&age=30&city=NY";
    
    http.appendData(request.c_str(), request.size());
    if (http.isComplete() == true){
        response =  http.getResponse();
        std::cout << "+++++ The Request is completed, Response :  " << std::endl;
        for(size_t i =0; i < response.size(); i++){
            std::cout << response[i] ;
        }
    }
}

void test_chunked_encoding() {
    std::cout << "\n=== Test 4: Chunked Encoding ===\n";
    
    GlobaConfig config = parseConfig("config.conf");
    HttpHandler http(config.servers[0]);
    std::vector<char> response;
    std::string request = "POST /upload/Files HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
        "Transfer-Encoding: chunked\r\n"
        "Connection: keep-alive\r\n"
        "\r\n";
        
        // First chunk (hex size: 0x9E = 158 bytes)
       std::string chunk1 =  "D1\r\n"
        "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
        "Content-Disposition: form-data; name=\"username\"\r\n"
        "\r\n"
        "john_doe\r\n"
        "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
        "Content-Disposition: form-data; name=\"email\"\r\n"
        "\r\n"
        "john@example.com\r\n"
        "\r\n";
        
        // Second chunk (hex size: 0xB4 = 180 bytes)
        std::string chunk2 = "EF\r\n"
        "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
        "Content-Disposition: form-data; name=\"file\"; filename=\"document.txt\"\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "This is the content of the uploaded file.\r\n"
        "It can contain multiple lines.\r\n"
        "And various characters!\r\n"
        "\r\n";
        
        // Third chunk (hex size: 0x2E = 46 bytes)
       std::string chunk3 = "2B\r\n"
        "------WebKitFormBoundary7MA4YWxkTrZu0gW--\r\n"
        "\r\n";
        
        // End of chunks
        std::string chunk4 = "0\r\n"
        "\r\n";
        // std::cout << "chunk1 size = " << chunk1.size() << std::endl;
        // std::cout << "chunk2 size = " << chunk2.size() << std::endl;
        // std::cout << "chunk3 size = " << chunk3.size() << std::endl;
        // std::cout << "chunk4 size = " << chunk4.size() << std::endl;
    // int i = 0;
    // while(!http.isComplete()){
    //     if (i == (int)request.length())
    //         break;
    //     // std::cout << "+++  i == " << i  << "length = " << str.length()<< std::endl;
    //     http.appendData(&request[i++], 1);
    // }
    request.append(chunk1).append(chunk2).append(chunk3).append(chunk4);
    http.appendData(request.c_str(), request.size());
    if (http.isComplete() == true){
        response =  http.getResponse();
        std::cout << "+++++ The Request is completed, Response :  " << std::endl;
        for(size_t i =0; i < response.size(); i++){
            std::cout << response[i] ;
        }
    }
}

void test_malformed_multipart() {
    std::cout << "\n=== Test 5: Malformed Multipart (Missing Boundary) ===\n";
    
    GlobaConfig config = parseConfig("config.conf");
    HttpHandler http(config.servers[0]);
    
    std::string request = 
        "POST /upload/Files HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: multipart/form-data\r\n"  // Missing boundary!
        "Content-Length: 43\r\n"
        "\r\n"
        "Some garbage data without proper boundaries";
    
    http.appendData(request.c_str(), request.size());
    std::vector<char> response;
    if (http.isComplete() == true){
        response =  http.getResponse();
        std::cout << "+++++ The Request is completed, Response :  " << std::endl;
        for(size_t i =0; i < response.size(); i++){
            std::cout << response[i] ;
        }
    }
    // Should return 400 Bad Request
}

void test_large_file() {
    std::cout << "\n=== Test 6: Large File Upload ===\n";
    
    GlobaConfig config = parseConfig("config.conf");
    HttpHandler http(config.servers[0]);
    std::vector<char> response;
    // Create a large string (simulating large file)
    std::string large_content(900, 'C'); // 5000 'A's
    
    std::string request = 
        "POST /upload/Files/abde HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " + std::to_string(large_content.size()) + "\r\n"
        "\r\n" +
        large_content;
    
    http.appendData(request.c_str(), request.size());
    if (http.isComplete() == true){
        response =  http.getResponse();
        std::cout << "+++++ The Request is completed, Response :  " << std::endl;
        for(size_t i =0; i < response.size(); i++){
            std::cout << response[i] ;
        }
    }
}

void test_empty_body() {
    std::cout << "\n=== Test 7: Empty Body ===\n";
    
    GlobaConfig config = parseConfig("config.conf");
    HttpHandler http(config.servers[0]);
    std::vector<char> response;
    std::string request = 
        "POST /upload/Files HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 0\r\n"
        "\r\n";
    
    http.appendData(request.c_str(), request.size());
    if (http.isComplete() == true){
        response =  http.getResponse();
        std::cout << "+++++ The Request is completed, Response :  " << std::endl;
        for(size_t i =0; i < response.size(); i++){
            std::cout << response[i] ;
        }
    }
}

int main() {
    std::cout << "Starting Comprehensive POST Tests...\n\n";
    
    // test_chunked_encoding();
    // test_multipart_basic();
    // test_multipart_no_filename();
    // test_url_encoded_form();
    // test_malformed_multipart();
    test_large_file();
    // test_empty_body();
    
    std::cout << "\n=== All Tests Completed ===\n";
    return 0;
}



