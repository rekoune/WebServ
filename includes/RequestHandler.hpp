# ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

# include "Request.hpp"
# include "config/configStructs.hpp"
# include "Utils.hpp"

class RequestHandler{
    private:
        Request req;
        ServerConfig server;
        HttpStatusCode findLocation(std::vector<LocationConfig> locations, std::string reqTarget, LocationConfig& resultLocation);
    public:
        RequestHandler();
        RequestHandler (const Request& req, const ServerConfig& server);
        RequestHandler (const RequestHandler& other);
        ~RequestHandler();
        RequestHandler& operator=(const RequestHandler& other);
        void setRequest(const Request& req);
        void setServer(const ServerConfig& server);
        HttpStatusCode handle();

};

# endif