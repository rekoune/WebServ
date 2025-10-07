
# ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "ResourceResolver.hpp"
# include <vector>
# include <map>
# include <string>
# include "Utils.hpp"
# include <sstream>
# include <fstream>
# include <dirent.h>
# include "struct.h"


class Response{
    private:
        HttpResourceInfo                    resInfo;
        ResElements                         resElements;
        std::vector<char>                   response;
        std::map<std::string, std::string>  fileTypes;
        
        void                                errorHandling();
        void                                successHandling();
        std::string                         getStatusLine();
        std::vector<char>                   generateErrorBody();
        std::map<std::string, std::string>  generateHeaders(std::map<std::string, std::string>& headers);
        std::string                         getStatusMessage(HttpStatusCode status);
        std::vector<char>                   getBodyFromFile(std::string& path);
        HttpStatusCode                      writeBodyInFile(std::string& path, std::vector<char>& body);
        void                                setFileTypes();
        void                                listDirectory();
        void                                handleGET();
        void                                handleDELETE();
        void                                handlePOST();
        void                                generateListingBody(DIR* dir);
        void                                buildResponse();
        HttpStatusCode                      getUploadPath();
        HttpStatusCode                      getPathType(std::string path, PathTypes& type);
        HttpStatusCode                      handleContentType();
        void                                setFullPathByType(std::string& path, PathTypes& pathType, std::string contentType);
        HttpStatusCode                      handleMultiParts(const std::vector<char>& body, std::string boundary);
        HttpStatusCode                      handleSinglePart(std::vector<char> singlePart, size_t size);
        HttpStatusCode                      extractHeaders(std::string bodyHeaders, std::map<std::string, std::string>& headers);

    public:
        Response();
        Response(const Response& other);
        Response(const HttpResourceInfo resInfo);
        ~Response();
        Response&           operator=(const Response& other);
        void                setResInfo(const HttpResourceInfo& info);
        std::vector<char>   getResponse() const;
        void                handle();
        void                clear();
};

# endif