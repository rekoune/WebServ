#ifndef CONFIGSTRUCTS_HPP
#define CONFIGSTRUCTS_HPP

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sstream>


struct LocationConfig
{
    std::string					path;
    std::string					root;
    std::vector<std::string>	index;

    bool						autoindex;
    
	std::string					upload_store;
    std::vector<std::string>	allowed_methods;
    std::string					cgi_pass;

    std::string                 redirection_url;
    int                         redirection_status;

    LocationConfig()
        :  index(1, "index.html") ,autoindex(false), redirection_url(""), redirection_status(0) {}

};

struct ServerConfig
{
    std::string					        host;
    std::vector<int>        	        port;
    std::string					        root;
    size_t						        client_max_body_size;
    std::map<int, std::string>	        error_pages;
	std::vector<LocationConfig>	        locations;
    std::map<std::string, std::string>  cgi_extension;
    ServerConfig() 
        : host("0.0.0.0"), port(1, 80){}
	
};



struct	GlobaConfig
{
	std::vector<ServerConfig>	servers;
};

GlobaConfig parseConfig(const std::string& configFilePath);

#endif