 #include "../../includes/config/configStructs.hpp"


bool isitnumber(std::string str)
{
	if ( str.empty())
		return false ;
	for (size_t i = 0; i < str.length(); i++)
	{
		if ( str[i] < '0' || str[i] > '9')
			return (false);
	}
	return true;
}	

bool isitspace(char c)
{
	if (c == 32 || (c >= 9 && c <= 13))
		return true;
	return false;
}

std::string cleanLine(std::string line)
{
    size_t  hashpos = line.find("#");
    if  (hashpos != std::string::npos)
        line.erase(hashpos);
    
    size_t	start = 0;
	while (start < line.size() && isitspace(line[start]) )
		start++;
	
	size_t	end = line.size();
	while (end > 0 && isitspace(line[end - 1]))
		end--;
	line = line.substr(start, end - start);
	return (line);
}

// bool is_str_exist_in_str_vec(std::string str, std::vector<std::string> str_vec);
bool	duplicated(std::string ip_str, std::string port_str, std::map<std::string, std::vector<std::string> > host_port)
{
	for (std::map<std::string, std::vector<std::string> >::iterator map_iter = host_port.begin(); map_iter != host_port.end(); map_iter++)
	{
		if (ip_str == map_iter->first)
		{
			for (std::vector<std::string>::iterator str_iter = map_iter->second.begin(); str_iter != map_iter->second.end(); str_iter++)
			{
				if (port_str == *str_iter)
					return true;
			}
		}
	}
	return false ;
}

// bool	duplicatedInOtherServ(std::vector<ServerConfig> servers, std::string ip_str, std::string port_str)
// {
// 	for (std::vector<ServerConfig>::iterator serv_iter = servers.begin(); serv_iter != servers.end(); serv_iter++)
// 	{
// 		if (duplicated(ip_str, port_str, serv_iter->host_port))
// 			return true;
// 	}
// 	return false ;
// }

bool	duplicated_server_name(std::string current_server_name, ServerConfig current_server)
{
	for (std::vector<std::string>::iterator serv_name_iter = current_server.server_name.begin(); serv_name_iter != current_server.server_name.end(); serv_name_iter++)
	{
		if ( current_server_name == *serv_name_iter)
			return true;
	}
	return false ;

}

bool	parseServerDirective(std::string line, ServerConfig& currentServer, std::vector<ServerConfig>& servers)
{
	(void)servers;
	bool	valid_directive = true;
	if (line.find("server") == 0 && line.find("server_name") == std::string::npos)
	{
		std::cerr << "CONFIG FILE ERROR : UNDIFINED KEYWORD IN THE SERVER BLOCK " << std::endl;
		return false;
	}
	if ( !line.empty() && line[line.size() - 1] != ';')
	{
		std::cerr << "CONFIG FILE ERROR : EVERY SERVER DIRECTIVE SHOULD END WITH ';' " << std::endl;
		return false;
	}
	//filling the struct of server
	std::size_t  pos = 0, end;
	while ((end = line.find(";", pos)) != std::string::npos)
	{
		std::string directive = cleanLine( line.substr(pos, end - pos));
		pos = end + 1;
		if ( directive.empty())
			continue;
		if (directive.find("server_name") == 0 && directive.length() > 11 && isitspace(directive[11]))
		{
			std::string	server_name;
			std::string i_server_name = cleanLine(directive.substr(11 + 1));
			std::istringstream iss(i_server_name);
			iss >> server_name;
			if ( !duplicated_server_name(server_name, currentServer))
				currentServer.server_name.push_back(server_name);
			std::string extra_parameter;
			iss >> extra_parameter;
			if (!extra_parameter.empty())
			{
				std::cerr << "CONFIG FILE ERROR : server_name should be one word " << std::endl;
				return false ;
			}
		}
		else if (directive.find ("index") == 0 && directive.length() > 5 &&  isitspace (directive[5]))
		{
			std::string	index_paths = cleanLine(directive.substr(5));
			std::string index_path;
			std::istringstream	iss(index_paths);
			while (iss >> index_path)
				currentServer.index.push_back(index_path);
		}
		else if (directive.find("root") == 0 && directive.length() > 4 &&  isitspace (directive[4])) {
			// std::cout << "=============IS SPACE A SAHBI \n";
            currentServer.root = cleanLine(directive.substr(4));
        }
        else if (directive.find("listen") == 0 && directive.length() > 6 && isitspace( directive[6])) {
			std::string value = cleanLine( directive.substr(6));
			std::size_t colon = value.find(":");
			if ( colon != std::string::npos)
			{
				std::string ip_str = value.substr(0, colon);
				std::string	port_str = cleanLine(value.substr(colon + 1));
				// std::cout << "===========================port_str: " << port_str << std::endl;
				if (!isitnumber(port_str))
				{
					std::cerr << "CONFIG FILE ERROR : port is not a valid number" << std::endl;
					return false ;
				}
				int port_int = std::atoi(port_str.c_str());
				if ( port_int < 1 || port_int > 65535 )
				{
					std::cerr << "CONFIG FILE ERROR : port is not a valid port number" << std::endl;
					return false ;
				}
				if ( !duplicated(ip_str, port_str, currentServer.host_port)/*  && !duplicatedInOtherServ(servers, ip_str, port_str) */)
				{
					currentServer.host_port[ip_str].push_back(port_str);
				}
				else
				{
					std::cout << "\033[36;1mDuplacated listen directive detected, one is ignored.\033[0m" << std::endl;
				}
			}
			else 
				currentServer.host_port[value].push_back("80");
        }
		// else if (directive.find("port") == 0)
		// {
		// 	std::string	value = cleanLine(directive.substr(4));
		// 	std::istringstream	iss(value);
		// 	int port;
		// 	while (iss >> port)
		// 		currentServer.port.push_back(port);
		// }
        else if (directive.find("client_max_body_size") == 0 && directive.length() > 20 &&  isitspace (directive[20])) 
		{
            // TODO: parse and convert to int, set currentServer.client_max_body_size
			// IM DOING IT NWO IGUESS 
			std::string value = cleanLine(directive.substr(21));
			if ( !value.empty())
			{
				size_t 		len = value.length();
				char 		suffix = value[len - 1];
				int			multiplier = 1;
				if ( suffix == 'k' || suffix == 'K')
				{
					multiplier = 1024;
					len--;
				}
				if ( suffix == 'm' || suffix == 'M')
				{
					multiplier = 1024 * 1024;
					len--;
				}
				if ( suffix == 'g' || suffix == 'G')
				{
					multiplier = 1024 * 1024 * 1024;
					len--;
				}
				std::string number_value = value.substr(0, len);
				if ( !isitnumber(number_value) )
				{
					std::cerr << "CONFIG FILE ERROR : INVALID CLIENT BODY SIZE" << std::endl;
					return false;
				}
				size_t number = std::atoll(number_value.c_str());
				// std::cout << "Nuuuuumber =====================: " << number << std::endl;
				currentServer.client_max_body_size = number * multiplier;
			}
        }
        else if (directive.find("error_page") == 0 && directive.length() > 10 &&  isitspace (directive[10])) 
		{
			std::string value = cleanLine(directive.substr(10));
			std::istringstream iss(value);
			std::vector<int> codes;
			std::string  code_str;
			int code;

			while (iss >> code_str)
			{
				if ( isitnumber(code_str))
				{
					code = std::atoi(code_str.c_str());
					if (code < 300 || code > 599)
					{
						std::cerr << "CONFIG FILE ERROR : HTTP ERROR NUMBER NOT VALID " << std::endl;
						return false;
					}
					codes.push_back(code);
				}
				else 
				{
					break;
				}
			}

			std::string path = code_str;
			if (path.empty() || codes.empty())
			{
				std::cerr << "CONFIG FILE ERROR : error_page missing code or path" << std::endl;
				return false;
			}
			std::string extra_parameter;
			iss >> extra_parameter;
			if (!extra_parameter.empty())
			{
				std::cerr << "CONFIG FILE ERROR : error_page too many path arguments" << std::endl;
				return false ;
			}
			for (size_t i = 0; i < codes.size(); i++)
				currentServer.error_pages[codes[i]] = path;
			
	    }
		else 
			valid_directive = false;
	}
	if ( valid_directive == false)
	{
		/* put back cerr  */std::cerr << "CONFIG FILE ERROR : UNDIFINED KEYWORD IN THE SERVER BLOCK. " << std::endl;
		return false;
	}
	return true;
}

bool	is_valid_status(int given_status)
{
	if ( given_status < 300 || given_status > 308)
		return false ;
	return true;
}

bool	parseLocationDirective(std::string line, LocationConfig& current_loc)
{
	bool	is_valid_directive = true;

	if ( !line.empty() && line[line.size() - 1] != ';')
	{
		std::cerr << "CONFIG FILE ERROR : EVERY SERVER DIRECTIVE SHOULD END WITH ';' " << std::endl;
		return false;
	}
	// std::cout <<"=================line :" << line << std::endl;
	size_t end, pos = 0;
	while ( (end = line.find(";", pos)) != std::string::npos)
	{
		std::string directive = cleanLine(line.substr(pos, end - pos));
		pos = end + 1;
		if ( directive.empty())
			continue;
		if (directive.find("root") == 0 && directive.length() > 4 &&  isitspace (directive[4]))
			current_loc.root = cleanLine(directive.substr(4));
		else if (directive.find ("index") == 0 && directive.length() > 5 &&  isitspace (directive[5]))
		{
			std::string	index_paths = cleanLine(directive.substr(5));
			std::string index_path;
			std::istringstream	iss(index_paths);
			while (iss >> index_path)
				current_loc.index.push_back(index_path);
		}
		else if ( directive.find("autoindex") == 0 && directive.length() > 9 &&  isitspace (directive[9]))
		{
			// std::istringstream iss(directive.substr(9));

			std::string 	keyword = cleanLine(directive.substr(9));
			if ( keyword == "on" || keyword == "ON")
				current_loc.autoindex = true;
			else if ( keyword == "off" || keyword == "OFF")
				current_loc.autoindex = false;
			else
			{
				std::cerr << "CONFIG FILE ERROR: INVALID AUTOINDEX DIRECTIVE" << std::endl;
				return false;				
			} 
		}
		else if ( directive.find("upload_store") == 0 && directive.length() > 12 &&  isitspace (directive[12]))
		{
			current_loc.upload_store = cleanLine(directive.substr(12));
			if (current_loc.upload_store[0] != '/')
			{
				std::string tmp("/");
				tmp += current_loc.upload_store;
				current_loc.upload_store = tmp;
			}
		}
		else if ( directive.find("allowed_methods") == 0 && directive.length() > 15 &&  isitspace (directive[15]))
		{
			std::string		methods = cleanLine(directive.substr(15));
			std::istringstream iss(methods);
			std::string 	method;
			current_loc.allowed_methods.clear();
			while ( iss >> method )
				current_loc.allowed_methods.push_back(method);
		}
		else if ( directive.find("return") == 0 && directive.length() > 6 &&  isitspace (directive[6]))
		{
			std::istringstream iss ( directive.substr(6));
			std::string status_str;
			iss >> status_str;
			bool is_number = isitnumber(status_str);
			if ( !is_number)
			{
				std::cerr << "CONFIG FILE ERROR: return keyword should be followed by redirection status number" << std::endl;
				return false ;
			}
			if (!is_valid_status(std::atoi(status_str.c_str())))
			{
				std::cerr << "CONFIG FILE ERROR: redirection status is not valid" << std::endl;
				return false ;
			}

			std::string redirection_url;	
			iss >> redirection_url;
			if (redirection_url.empty())
			{
				std::cerr << "CONFIG FILE ERROR: redirection should have a url after redirection status number " << std::endl;
				return false ;
			}
			std::string extra_parameter;
			iss >> extra_parameter;
			if (!extra_parameter.empty())
			{
				std::cerr << "CONFIG FILE ERROR: Redirection sytanx : return  <redirc_status> <redirec_path>" << std::endl;
				return false ;
			}
			current_loc.redirection_url = redirection_url;
			current_loc.redirection_status = std::atoi(status_str.c_str());
		}
		else if (directive.find("cgi_extension") == 0 && directive.length() > 13 &&  isitspace (directive[13]))
		{
			std::string	value = cleanLine(directive.substr(13));
			std::istringstream	iss(value);
			std::string	extension;
			std::string extra;
			iss >> extension;
			iss >> extra;
			if ( !extra.empty())
			{
				std::cerr << " CONFIG FILE ERROR: extra argument in cgi_extension" << std::endl;
				return false ;
			}
			current_loc.cgi_extension.push_back(extension);
		}
		else 
		{
			is_valid_directive = false ;
		}
	}
	if (!is_valid_directive)
	{
		std::cerr << "CONFIG FILE ERROR: invalid location block keyword " << std::endl;
		return false ;
	}
	return true;
}

searchServerStatus	searchForServer(bool &in_server_block,  std::string& line, ServerConfig& currentserver, GlobaConfig& globalConfig, bool& waiting_for_brace)
{
	if ((line.find("server") == 0 && line.find("{") != std::string::npos))
	{
		// std::cout << "WE ARE IN THE SERVER BLOCK \n"; // for debug
		//idk if i need to set the flags to true or no TO SEE
		in_server_block = true;
		//ADD A NEW SERVER TO THE VECTOR
		//SKIP the {
		size_t		brace_pos = line.find("{");
		std::string	remaining = line.substr(brace_pos + 1);
		remaining = cleanLine(remaining);
		if (!remaining.empty())
		{
			if ( remaining  == "}")
			{
				in_server_block = false;
				globalConfig.servers.push_back(currentserver);
				currentserver = ServerConfig();
			}
			else 
				parseServerDirective(remaining, currentserver, globalConfig.servers);
		}
		return CONTINUE_SRV ;
	}
	else if ( line.find("server") == 0 &&  line.find("{") == std::string::npos)
	{
		// std::cout << "SERVER FOUND, WAITING FOR BRACE  \n"; // for debug
		waiting_for_brace = true;
		return CONTINUE_SRV ;
	}
	else if (line.find("{") == 0 && waiting_for_brace)
	{
		// std::cout << "FOUND THE '{' WE ARE IN THE SERVER BLOCK \n"; // for debug
		in_server_block = true;
		waiting_for_brace = false ; 
		//SKIP the {
		size_t		brace_pos = line.find("{");
		std::string	remaining = line.substr(brace_pos + 1);
		remaining = cleanLine(remaining);
		if (!remaining.empty())
		{
			if ( remaining  == "}")
			{
				in_server_block = false;
				globalConfig.servers.push_back(currentserver);
			}
			else 
				parseServerDirective(remaining , currentserver, globalConfig.servers);
		}
		return CONTINUE_SRV ;
	}
	else 
	{
		std::cerr << "CONFIG FILE ERROR: SYNTAX ERROR uknown config keyword\n";
		return FALSE_RETURN;
	}
}


void	fillDefaults(GlobaConfig& globalConfig)
{
	for (std::vector<ServerConfig>::iterator serv_iter = globalConfig.servers.begin(); serv_iter != globalConfig.servers.end(); serv_iter++)
	{
		if (serv_iter->host_port.empty())
			serv_iter->host_port["0.0.0.0"].push_back("8000");
		if (serv_iter->index.empty())
			serv_iter->index.push_back("index.html");
		for (std::vector<LocationConfig>::iterator loc_iter = serv_iter->locations.begin(); loc_iter != serv_iter->locations.end(); loc_iter++)
		{
			if (loc_iter->index.empty())
			{
				if ( !(serv_iter->index.empty()))
					loc_iter->index = serv_iter->index;
				else 
				{
						std::cout << "hheeeeere\n";
					loc_iter->index.push_back("index.html");
				}
			}
		}

		// if (serv_iter->root.empty())
		// 	serv_iter->root = "/";
		// if (!serv_iter->client_max_body_size)
		// 	serv_iter->client_max_body_size = 1024 * 1024;
		// serv_iter->error_pages	
	}
}

bool	parseLocationBlock(ServerConfig&	currentserver, std::ifstream& file, std::string& line/* , bool& is_loc_brace_closed */)
{
	bool			is_loc_brace_closed = false;


	LocationConfig current_loc;
	size_t	loc_start = line.find("location") + 8;
	size_t	brace_pos = line.find("{", loc_start);
	// if ( loc_start == std::string::npos )
	// {
	// 	std::cerr << "CONFIG FILE ERROR: Malformed location block better use opening brace in the first line \n";
	// 	continue ;  
	// }
	// current_loc.path = cleanLine(brace_pos != std::string::npos ? line.substr(loc_start, brace_pos - loc_start)
	// 	: cleanLine(line.substr(loc_start)));
	if ( brace_pos != std::string::npos)
	{
		std::string after_brace = cleanLine(line.substr(brace_pos + 1));				//throwig error if anything after {
		if  (!after_brace.empty())
		{
			std::cerr << "CONFIG FILE ERROR: Syntax : location <path/> {" << std::endl;
			return false;
		}
		current_loc.path = cleanLine (line.substr(loc_start, brace_pos - loc_start));
		if (current_loc.path[0] != '/')
		{
			std::string tmp("/");
			tmp += current_loc.path;
			current_loc.path =  tmp;

		}
	}
	else 
	{
		 current_loc.path = cleanLine(line.substr(loc_start));
	}
	if ( current_loc.path.empty() )
	{
		// here i can add Skipping empty lines to find path but no need
		std::cerr << "CONFIG FILE ERROR: no path for location - syntax :<location> </Path/> <{> \n";
		return false;
	}
	if (brace_pos == std::string::npos)
	{
		while (std::getline(file, line))
		{
			line = cleanLine(line);
			if ( line.empty())
				continue;
			else if ( line == "{")
				break ;
			else
			{
				std::cerr << "CONFIG FILE ERROR: no <{> after location and path" << std::endl;
				return false;
			}
		}
	}
	// needs to check is anything after "{"
	while ( std::getline(file, line))
	{
		line = cleanLine(line);
		if ( line.empty())
			continue;
		if ( line == "}")
		{
			is_loc_brace_closed = true;
			break;				
		}
		if (!parseLocationDirective(line, current_loc))
			return false;
	}
	// std::cout << "is_LOC_BRACE_CLOSED :" << is_loc_brace_closed << std::endl; // debug
	if ( file.eof() && is_loc_brace_closed == false)
	{
		std::cerr << "CONFIG FILE ERROR: location braces not close" << std::endl;
		return false;
	}
	if (current_loc.root.empty())
		current_loc.root = currentserver.root;
	currentserver.locations.push_back(current_loc);
	return true;
}

bool parseServerBlock(GlobaConfig& globalConfig, std::ifstream& file, bool& waiting_for_brace, bool& is_serv_brace_closed)
{
    std::string 	line;

	bool	in_server_block = false;

	
	ServerConfig currentserver;
    
	while (std::getline(file, line))
    {
		// std::cout << "we reading agin and eof: " << file.eof() << std::endl;
		line = cleanLine(line);     //	triming and removing comments 
		if (line.empty())
			continue;
		// std::cout << line << std::endl; 	// Print for debug
		if (!in_server_block)
		{
			searchServerStatus status = searchForServer(in_server_block, line, currentserver, globalConfig,  waiting_for_brace);
			if ( status == CONTINUE_SRV)
				continue;
			else if (status == FALSE_RETURN)
				return false ;                                                    
		}
		else 
		{
			// std::cout << "WE ARE IN THE SERVER LINE: " << line << std::endl; // for debug
			
			is_serv_brace_closed = false;
			if ( line == "}")
			{
				is_serv_brace_closed = true;
				globalConfig.servers.push_back(currentserver);
				currentserver = ServerConfig();
				in_server_block = false;
			}
			else if (line.find("location") == 0)
			{
				if (!parseLocationBlock(currentserver, file, line))
					return false;
			}
			else if (!parseServerDirective(line, currentserver, globalConfig.servers))
				return false;

			// std::cout << "is_serv_brace_closed :" <<  is_serv_brace_closed << std::endl;
			// std::cout << "line                 :" << line << std::endl;

			// if ( file.eof() && !is_serv_brace_closed)
			// {
			// 	std::cerr << "CONFIG FILE ERROR: server braces not close" << std::endl;
			// 	return false;
			// }
		}
    }
	return true;
}

bool parseConfig(const std::string& configFilePath, GlobaConfig& globalConfig)
{
	bool			is_serv_brace_closed = false;
    std::ifstream 	file(configFilePath.c_str());

	bool	waiting_for_brace = false;

    if (!file.is_open())
    {
        std::cerr << "CONFIG FILE ERROR: error opening config file: " << configFilePath << std::endl;
        return false;
    }
	if ( !parseServerBlock(globalConfig, file, waiting_for_brace, is_serv_brace_closed))
		return false;
	if (waiting_for_brace)
		std::cerr << "CONFIG FILE ERROR: NO SERVER BLOCK\n";
	if (!is_serv_brace_closed /* || in_server_block */)
	{
		std::cerr << "CONFIG FILE ERROR: server braces not close" << std::endl;
		return false;
	}
	fillDefaults(globalConfig);
	if ( !validateConfig(globalConfig))
		return false ;
	return true;
}