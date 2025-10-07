# ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

// # include "Structs.hpp"
# include "Enums.hpp"
# include <map>
# include <vector>
# include <deque>
# include "ResourceResolver.hpp"
# include "Utils.hpp"
# include "ResourceResolver.hpp"
# include "UploadHandler.hpp"

class RequestParser{
    private:
        // std::string                         req;
        RequestLine                         requestLine;
        std::map<std::string, std::string>  headers;
        std::vector<char>                   body;
        ParseState                          parseState;
        ServerConfig                        server;
        ResourceResolver                    resourceResolver;
        HttpResourceInfo                    resInfo;
        UploadHandler                       uploadHandler;

        long long                           bodyMaxSize;
        size_t                              clientMaxBodySize;

        HttpStatusCode parseRequestLine(std::string& reqLine);
        HttpStatusCode parseRequestHeaders(std::string& req);
        HttpStatusCode setMethod(std::string& method);
        HttpStatusCode setTarget(std::string& target);
        HttpStatusCode setHttpVersion(std::string& httpVersion);
        ParseState     checkPostHeaders(HttpStatusCode& status);
        

    public:
        RequestParser();
        ~RequestParser();
        RequestParser( const RequestParser& other);
        RequestParser( const ServerConfig& server);
        RequestParser& operator=( const RequestParser& other);
        RequestLine                         getRequestLine() const;
        std::map<std::string, std::string>  getHeaders() const;
        HttpResourceInfo                    getResourceInfo()const;
        HttpStatusCode                      parseRequest(std::string req);
        HttpStatusCode                      appendData(const char* _data, size_t size);
        void                                setClientMaxBody(size_t clientMaxBodySize);
        bool                                isComplete();


};

# endif