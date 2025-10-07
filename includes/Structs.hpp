#ifndef STRUCTS_HPP
#define STRUCTS_HPP

# include <iostream>
# include "Enums.hpp"
# include "config/configStructs.hpp"

struct RequestLine{
    std::string method;
    std::string target;
    std::string httpVersion;
};
struct HttpResourceInfo{
    HttpStatusCode status;
    std::string     path;
    PathTypes       type;
    LocationConfig  location;
    ServerConfig    server;
    RequestLine     reqLine;
    std::string     method;
    std::map<std::string, std::string>  headers;

    HttpResourceInfo(): status(OK){};
};


struct RequestContext{
	RequestLine 						req_line;
	std::vector<char>                   body;


	std::string 						script_path;
    std::map<std::string, std::string>  headers;
	std::string							query;
	std::string							script_name;
	std::string							server_name;

};

struct ResElements{
    std::string statusLine;
    std::map<std::string, std::string> headers;
    std::vector<char> body;
};

# endif