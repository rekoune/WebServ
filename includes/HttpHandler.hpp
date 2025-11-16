# ifndef HTTPHANDLER_HPP
# define HTTPHANDLER_HPP

# include "RequestParser.hpp"
// # include "ResourceResolver.hpp"
# include "Response.hpp"

class HttpHandler{
    private:
        RequestParser reqParser;
        ServerConfig server;
        ResourceResolver reqHandler;
        HttpResourceInfo resInfo;
        Response response;
        bool        sameReq;
        bool        sameRes;

    public:
        HttpHandler();
        HttpHandler(const ServerConfig& server);
        HttpHandler (const HttpHandler& other);
        ~HttpHandler();
        HttpHandler& operator=(const HttpHandler& other);
        void setServer(const ServerConfig& server);
        void appendData(const char* data, size_t size);
        bool isComplete();
        std::vector<char> getResponse();
        std::vector<char> getStatusResponse(const HttpStatusCode& statusCode);
        bool    isKeepAlive();
        bool    isResDone();
        int     isScript();
        void    cleanCGI();
};

# endif