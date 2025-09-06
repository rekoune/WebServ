# ifndef HTTPHANDLER_HPP
# define HTTPHANDLER_HPP

# include "Request.hpp"
# include "RequestHandler.hpp"

class HttpHandler{
    private:
        Request req;
        ServerConfig server;
        RequestHandler reqHandler;
    public:
        HttpHandler();
        HttpHandler (std::vector<char> req, const ServerConfig& server);
        HttpHandler (const HttpHandler& other);
        ~HttpHandler();
        HttpHandler& operator=(const HttpHandler& other);
        void setRequest(const Request& req);
        void setServer(const ServerConfig& server);
        void setRequestHandler(const RequestHandler& reqHandler);
        HttpStatusCode handel();
};

# endif