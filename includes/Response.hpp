# ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "RequestHandler.hpp"
# include <vector>
# include <map>
# include <string>
# include "Utils.hpp"
# include <sstream>

struct ResElements{
    std::string statusLine;
    std::map<std::string, std::string> headers;
    std::vector<char> body;
};

class Response{
    private:
        HttpResponseInfo    resInfo;
        ResElements         resElements;
        std::vector<char>   response;
        
        void                                errorHandling();
        void                                successHandling();
        std::string                         getStatusLine();
        void                                generateStatusLine(std::string& str, long status, std::string message);
        std::vector<char>                   generateErrorBody();
        std::map<std::string, std::string>  generateErrorHeaders();
        std::string                         getStatusMessage(HttpStatusCode status);

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