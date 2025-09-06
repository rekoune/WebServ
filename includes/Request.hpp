# ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include <sstream>
# include <exception>
# include <deque>
# include <map>
# include <vector>
# include "Utils.hpp"
# include "config/configStructs.hpp"

enum HttpStatusCode{
    CONTINUE = 100, 
    
    OK = 200,
    CREATED = 201,
    NO_CONTENT = 204,

    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    CONTENT_TOO_LARGE= 413,

    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    HTTP_VERSION_NOT_SUPPORTED = 505
};

typedef struct ReqLine{
    std::string method;
    std::string target;
    std::string httpVersion;
} RequestLine;

class Request{
    private:
        std::string req;
        RequestLine requestLine;
        std::map<std::string, std::string> headers;
        std::vector<char> body;
        size_t bodyIndex;

        HttpStatusCode parseRequestLine(std::string& reqLine);
        HttpStatusCode parseRequestHeaders(std::stringstream& req);
        HttpStatusCode parseBody();
        HttpStatusCode setMethod(std::string& method);
        HttpStatusCode setTarget(std::string& target);
        HttpStatusCode setHttpVersion(std::string& httpVersion);
        int            splitHttpRequest(std::vector<char>& req);
    public:
        Request();
        Request(std::vector<char> req);
        ~Request();
        Request( const Request& other);
        Request& operator=( const Request& other);

        HttpStatusCode                      parseRequest();
        void                                setRequest(std::vector<char> req);
        RequestLine                         getRequestLine() const;
        std::map<std::string, std::string>  getHeaders() const;
        std::vector<char>                   getBody() const;
        //delete
        void printHeaders();
        void printBody();

};

# endif