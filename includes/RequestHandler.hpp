# ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

# include "Request.hpp"
# include "config/configStructs.hpp"
# include "Utils.hpp"
# include <sys/stat.h>
# include <unistd.h>
# include <vector>

enum PathTypes{
    F,
    SCRIPT,
    DIR,
};

class RequestHandler{
    private:
        Request req;
        ServerConfig server;
        HttpStatusCode  findLocation(std::vector<LocationConfig> locations, std::string reqTarget, LocationConfig& resultLocation);
        HttpStatusCode  resolveResourceType(std::string& path, PathTypes& pathType, LocationConfig& location);
        HttpStatusCode  dirHandling(std::string& path, PathTypes& pathType, LocationConfig& location);
        HttpStatusCode  fileHandling(std::string& path, PathTypes& pathType, LocationConfig& location);
        bool            isScript(std::string& path, LocationConfig& location);
        HttpStatusCode  isMethodAllowed(std::vector<std::string> allowedMethods, std::string reqMethod);
        
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