#ifndef STRUCTS_HPP
#define STRUCTS_HPP

# include <iostream>
# include "Enums.hpp"
# include "config/configStructs.hpp"

struct RequestLine{
    std::string method;
    std::string target;
    std::string httpVersion;
    std::string query;
};
struct HttpResourceInfo{
    HttpStatusCode status;
    std::string     path;
    std::string     cgiBodyPath;
    PathTypes       type;
    LocationConfig  location;
    ServerConfig    server;
    RequestLine     reqLine;
    std::string     method;
    int             cgiFD;
    std::map<std::string, std::string>  headers;

    HttpResourceInfo(): status(OK), cgiFD(-1){};
};


struct RequestContext{
	RequestLine 						req_line;
	std::vector<char>                   body;
    std::string 						script_path;
    std::map<std::string, std::string>  headers;

};

struct CgiResult{
    std::map<std::string, std::string> headers;
    std::vector<char> body;
	HttpStatusCode status;
};

struct ResElements{
    std::string statusLine;
    std::map<std::string, std::string> headers;
    std::vector<char> body;
    std::string       path;
    size_t            position;
};

# endif