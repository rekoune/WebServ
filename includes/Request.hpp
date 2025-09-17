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
# include <unistd.h>

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

enum ParseState {
    PARSE_START,       
    PARSE_HEADERS,     
    PARSE_BODY_LENGTH, 
    PARSE_BODY_CHUNKED,
    PARSE_COMPLETE,    
    PARSE_ERROR        
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
        // size_t bodyIndex;
        ParseState parseState;
        long      bodyMaxSize;
        std::vector<char> chunkBody;
        size_t clientMaxBodySize;

        HttpStatusCode parseRequestLine(std::string& reqLine);
        HttpStatusCode parseRequestHeaders(std::stringstream& req);
        // HttpStatusCode parseBody();
        HttpStatusCode setMethod(std::string& method);
        HttpStatusCode setTarget(std::string& target);
        HttpStatusCode setHttpVersion(std::string& httpVersion);
        // int            splitHttpRequest(std::vector<char>& req);
        // ParseState     getBodyType();
        // ParseState     unchunk();

        ParseState     unchunk();
        ParseState     singleChunk(std::vector<char> oneChunk, size_t size);

        // ParseState     singleChunk(std::vector<char> oneChunk, long sizePos, long bodyPos);
        ParseState     checkPostHeaders(HttpStatusCode& status);
        void           appendBody(const char* _data, size_t size, HttpStatusCode& status);


    public:
        Request();
        // Request(std::vector<char> req);
        ~Request();
        Request( const Request& other);
        Request& operator=( const Request& other);

        HttpStatusCode                      parseRequest();
        // void                                setRequest(std::vector<char> req);
        RequestLine                         getRequestLine() const;
        std::map<std::string, std::string>  getHeaders() const;
        std::vector<char>                   getBody() const;
        HttpStatusCode                      appendData(const char* _data, size_t size);
        bool                                isComplete();
        void                                setClientMaxBody(size_t clientMaxBodySize);
        //delete
        void printHeaders();
        void printBody();

};

# endif