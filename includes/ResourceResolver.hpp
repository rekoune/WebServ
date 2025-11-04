# ifndef RESOURCERESOLVER_HPP
# define RESOURCERESOLVER_HPP

// # include "HttpStatusCode.hpp"
// # include "Request.hpp"
# include "config/configStructs.hpp"
# include "Utils.hpp"
# include <sys/stat.h>
# include <unistd.h>
# include <vector>
# include "Enums.hpp"
# include "Structs.hpp"
 


class ResourceResolver{
    private:
        RequestLine reqLine;
        std::vector<LocationConfig> locations;
        HttpResourceInfo resInfo;

        HttpStatusCode  findLocation(std::vector<LocationConfig> locations, std::string reqTarget, LocationConfig& resultLocation);
        HttpStatusCode  resolveResourceType(std::string& path, PathTypes& pathType, LocationConfig& location);
        HttpStatusCode  dirHandling(std::string& path, PathTypes& pathType, LocationConfig& location);
        HttpStatusCode  fileHandling(std::string& path, PathTypes& pathType);
        HttpStatusCode  isMethodAllowed(std::vector<std::string> allowedMethods, std::string reqMethod);
        
    public:
        ResourceResolver();
        ResourceResolver (const RequestLine& reqLine, const std::vector<LocationConfig>& locations);
        ResourceResolver (const ResourceResolver& other);
        ~ResourceResolver();
        ResourceResolver& operator=(const ResourceResolver& other);
        void setRequestLine(const RequestLine& req);
        void setLocations(const std::vector<LocationConfig>& locations);
        void setServer(const ServerConfig&    server);
        HttpResourceInfo handle(std::map<std::string,std::string> headers);
        HttpResourceInfo getResponseInfo() const;

};

# endif