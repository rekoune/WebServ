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

    public:
        HttpHandler();
        // HttpHandler (std::vector<char> req, const ServerConfig& server);
        HttpHandler(const ServerConfig& server);
        HttpHandler (const HttpHandler& other);
        ~HttpHandler();
        HttpHandler& operator=(const HttpHandler& other);
        // void setRequest(const Request& req);
        void setServer(const ServerConfig& server);
        // void setResourceResolver(const ResourceResolver& reqHandler);
        // void handle();
        void appendData(const char* data, size_t size);
        bool isComplete();
        std::vector<char> getResponse();
        bool isKeepAlive();
};

# endif