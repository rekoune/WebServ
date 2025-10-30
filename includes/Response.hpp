
# ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "ResourceResolver.hpp"
# include "cgi/CgiExecutor.hpp"
# include "GetHandler.hpp"
# include <vector>
# include <map>
# include <string>
# include "Utils.hpp"
# include <sstream>
# include <fstream>
# include <dirent.h>
# include "Structs.hpp"

# define DATA_SIZE (1 * 1024 * 1024)

class Response{
    private:
        HttpResourceInfo                    resInfo;
        ResElements                         resElements;
        std::vector<char>                   response;
        std::map<std::string, std::string>  fileTypes;
        bool                                keepAlive;
        bool                                done;
        GetHandler                          getHandler;
        RequestContext                      cgiInfo;
        CgiExecutor                         cgiExecutor;
        int                                 cgiFd;

        void                                errorHandling();
        void                                successHandling();
        std::string                         getStatusLine();
        std::vector<char>                   generateErrorBody();
        std::map<std::string, std::string>  generateHeaders(std::map<std::string, std::string>& headers);
        std::string                         getStatusMessage(HttpStatusCode status);
        void                                getBodyFromFile(std::string& path);
        HttpStatusCode                      writeBodyInFile(std::string& path, std::vector<char>& body);
        void                                setFileTypes();
        void                                listDirectory();
        void                                handleGET();
        void                                handleDELETE();
        void                                generateListingBody(DIR* dir);
        void                                buildResponse();
        HttpStatusCode                      getPathType(std::string path, PathTypes& type);
        void                                setFullPathByType(std::string& path, PathTypes& pathType, std::string contentType);
        int                                 isCgi();

    public:
        Response();
        Response(const Response& other);
        Response(const HttpResourceInfo resInfo);
        ~Response();
        bool                isKeepAlive();
        Response&           operator=(const Response& other);
        void                setCgiExecutor(const CgiExecutor& cgiExecutor);
        void                setResInfo(const HttpResourceInfo& info);
        std::vector<char>   getResponse();
        void                handle();
        void                clear();
        bool                isDone();
};

# endif