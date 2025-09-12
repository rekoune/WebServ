# ifndef HTTPHANDLER_HPP
# define HTTPHANDLER_HPP

# include "Request.hpp"
# include "RequestHandler.hpp"
# include "Response.hpp"

class HttpHandler{
    private:
        Request req;
        ServerConfig server;
        RequestHandler reqHandler;
        HttpResponseInfo resInfo;
        Response response;

    public:
        HttpHandler();
        HttpHandler (std::vector<char> req, const ServerConfig& server);
        HttpHandler (const HttpHandler& other);
        ~HttpHandler();
        HttpHandler& operator=(const HttpHandler& other);
        void setRequest(const Request& req);
        void setServer(const ServerConfig& server);
        void setRequestHandler(const RequestHandler& reqHandler);
        void handel();
        void appendData(const char* data, size_t size);
        bool isComplete();
        std::vector<char> getResponse();
};

# endif