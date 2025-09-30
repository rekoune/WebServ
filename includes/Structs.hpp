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


# endif