# ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "RequestHandler.hpp"
# include <vector>
# include <map>
# include <string>
# include "Utils.hpp"
# include <sstream>
# include <fstream>
# include <dirent.h>

struct ResElements{
    std::string statusLine;
    std::map<std::string, std::string> headers;
    std::vector<char> body;
};

class Response{
    private:
        HttpResponseInfo                    resInfo;
        ResElements                         resElements;
        std::vector<char>                   response;
        std::map<std::string, std::string>  fileTypes;
        
        void                                errorHandling();
        void                                successHandling();
        std::string                         getStatusLine();
        std::vector<char>                   generateErrorBody();
        std::map<std::string, std::string>  generateHeaders();
        std::string                         getStatusMessage(HttpStatusCode status);
        std::vector<char>                   getBodyFromFile(std::string& path);
        HttpStatusCode                      writeBodyInFile(std::string& path, std::vector<char>& body);
        void                                setFileTypes();
        void                                listDirectory();
        void                                handelGET();
        void                                handelDELETE();
        void                                handelPOST();
        void                                generateListingBody(DIR* dir);
        void                                buildResponse();
        HttpStatusCode                      getUploadPath();
        HttpStatusCode                      getPathType(std::string path, PathTypes& type);

    public:
        Response();
        Response(const Response& other);
        Response(const HttpResponseInfo resInfo);
        ~Response();
        Response&           operator=(const Response& other);
        void                setResInfo(const HttpResponseInfo& info);
        std::vector<char>   getResponse() const;
        void                handel();
};

# endif