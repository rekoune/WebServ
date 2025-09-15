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
void	configStructInit(std::string line, ServerConfig& currentServer)
{
	//filling the struct of server
	std::size_t  pos = 0, end;
	while ((end = line.find(";", pos)) != std::string::npos)
	{
		std::string directive = cleanLine( line.substr(pos, end - pos));
		pos = end + 1;
		if ( directive.empty())
			continue;
		if (directive.find("root") == 0) {
            currentServer.root = cleanLine(directive.substr(directive.find(" ") + 1));
        }
        else if (directive.find("listen") == 0) {
			std::string value = cleanLine( directive.substr(6));
			// std::size_t colon = value.find(':');
			// std::cout << colon << std::endl;
			// if ( colon != std::string::npos)
			// {
			// 	currentServer.host = value.substr(0, colon);
			// 	currentServer.port = std::atoi(value.substr(colon + 1).c_str());  // I HANDLE HERE THE HOST AND PORTS AS PAIRS (127.0.0.1:8080)
			// }
			// else 
			// {
			// 	currentServer.host = value; 
			// 	currentServer.port = 80; 
			// }
			currentServer.host = cleanLine(directive.substr(directive.find(" ") + 1));
        }
		else if (directive.find("port") == 0)
		{
			std::string	value = cleanLine(directive.substr(4));
			std::istringstream	iss(value);
			int port;
			while (iss >> port)
				currentServer.port.push_back(port);
		}
        else if (directive.find("client_max_body_size") == 0) 
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
				size_t number = std::atoi(number_value.c_str());
				currentServer.client_max_body_size = number * multiplier;
			}
        }
        else if (directive.find("error_page") == 0) 
		{
			std::string value = cleanLine(directive.substr(10));
			std::istringstream iss(value);
			int code;
			std::string path;
			iss >> code >> path;
			currentServer.error_pages[code] = path;

	    }
		else if (directive.find("cgi_extension") == 0)
		{
			std::string	value = cleanLine(directive.substr(13));
			std::istringstream	iss(value);
			std::string	extension, interperter;
			iss >> extension >> interperter;
			currentServer.cgi_extension[extension] = interperter;
		}
	}	
}


void	parseLocationBlock(std::string line, LocationConfig& current_loc)
{
	size_t end, pos = 0;
	while ( (end = line.find(";", pos)) != std::string::npos)
	{
		std::string directive = cleanLine(line.substr(pos, end - pos));
		pos = end + 1;
		if ( directive.empty())
			continue;
		if (directive.find("root") == 0)
			current_loc.root = cleanLine(directive.substr(directive.find(" ") + 1));
		else if (directive.find ("index") == 0)
		{
			std::string	index_paths = cleanLine(directive.substr(directive.find(" ") + 1));
			std::string index_path;
			std::istringstream	iss(index_paths);
			while (iss >> index_path)
				current_loc.index.push_back(index_path);
		}
		else if ( directive.find("autoindex") == 0)
			current_loc.autoindex = (directive.find("on") != std::string::npos);
		else if ( directive.find("upload_store") == 0)
			current_loc.upload_store = cleanLine(directive.substr(directive.find(" ") + 1));
		else if ( directive.find("cgi_pass") == 0)
			current_loc.cgi_pass = cleanLine(directive.substr(directive.find(" ") + 1)); 
		else if ( directive.find("allowed_methods") == 0)
		{
			std::string		methods = cleanLine(directive.substr(directive.find(" ") + 1));
			std::istringstream iss(methods);
			std::string 	method;
			current_loc.allowed_methods.clear();
			while ( iss >> method )
				current_loc.allowed_methods.push_back(method);
		}
		else if ( directive.find("return") == 0)
		{
			std::istringstream iss ( directive.substr(6));
			std::string status_str;
			iss >> status_str;
			bool is_number = isitnumber(status_str);
			if ( !is_number)
			{
				std::cerr << "return keyword should be followed by redirection status number" << std::endl;
				return ;
			}
			std::string redirection_url;	
			iss >> redirection_url;
			if (redirection_url.empty())
			{
				std::cerr << "redirection should have a url after redirection status number " << std::endl;
				return ;
			}
			current_loc.redirection_url = redirection_url;
			current_loc.redirection_status = std::atoi(status_str.c_str());
		}
	}
}

GlobaConfig parseConfig(const std::string& configFilePath)
{
    GlobaConfig globalConfig;
    std::string line;
    std::ifstream file(configFilePath.c_str());
    if (!file.is_open())
    {
        std::cerr << "error opening config file: " << configFilePath << std::endl;
        return globalConfig;
    }
	bool	in_server_block = false;
	bool	waiting_for_brace = false;
	ServerConfig currentserver;
    while (std::getline(file, line))
    {
		line = cleanLine(line);     //	triming and removing comments 
		if (line.empty())           //  checking if the line is empty
			continue;
		// std::cout << line << std::endl; 	// Print for debugging 
		if (!in_server_block)
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
						in_server_block = false;
					else 
						configStructInit(remaining, currentserver);
				}
				continue ;
			}
			else if ( line.find("server") == 0 &&  line.find("{") == std::string::npos)
			{
				// std::cout << "SERVER FOUND, WAITING FOR BRACE  \n"; // for debug

				waiting_for_brace = true;
				continue ;
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
						configStructInit(remaining , currentserver);
				}
				continue ;
			}
			else 
			{
				std::cerr << "SYNTAX ERROR\n";
				break;
			}
		}
		else 
		{
			// std::cout << "WE ARE IN THE SERVER LINE: " << line << std::endl; // for debug
			if ( line == "}")
			{
				globalConfig.servers.push_back(currentserver);
				currentserver = ServerConfig();
				in_server_block = false;
			}
			else if (line.find("location") == 0)
			{
				LocationConfig current_loc;

				size_t	loc_start = line.find("location") + 8;
				size_t	brace_pos = line.find("{", loc_start);
				if ( loc_start == std::string::npos)
				{
					std::cerr << "Malformed location block better use opening brace in the first line \n";
					continue ;  
				}
				current_loc.path = cleanLine(brace_pos != std::string::npos ? line.substr(loc_start, brace_pos - loc_start)
					: line.substr(loc_start));
				
				if (brace_pos == std::string::npos)
				{
					while (std::getline(file, line))
					{
						line = cleanLine(line);
						if ( line.empty())
							continue;
						if ( line == "{")
							break ; 
					}
				}
				while ( std::getline(file, line))
				{
					line = cleanLine(line);
					if ( line.empty())
						continue;
					if ( line == "}")
						break;				
					parseLocationBlock(line, current_loc);
				}
				if (current_loc.root.empty())
					current_loc.root = current_loc.path;
				currentserver.locations.push_back(current_loc);
			}
			else 
				configStructInit(line, currentserver);
		}
    }

	if (waiting_for_brace)
		std::cerr << "ERROR : NO SERVER BLOCK\n";
    return globalConfig;
}