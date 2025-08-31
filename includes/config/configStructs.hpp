#ifndef CONFIGSTRUCTS_HPP
#define CONFIGSTRUCTS_HPP

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>


struct LocationConfig
{
    std::string					path;
    std::string					root;
    std::string					index;

    bool						autoindex;
    
	std::string					upload_store;
    std::vector<std::string>	allowed_methods;
    std::string					cgi_pass;

};

struct ServerConfig 
{
    std::string					host;
    int        					port;
    std::string					root;
    size_t						client_max_body_size;
    std::map<int, std::string>	error_pages;
	std::vector<LocationConfig>	locations;
	
};



struct	GlobaConfig
{
	std::vector<ServerConfig>	servers;
};

GlobaConfig parseConfig(const std::string& configFilePath);

#endif