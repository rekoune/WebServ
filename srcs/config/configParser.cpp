#include "../../includes/config/configStructs.hpp"

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
			std::size_t colon = value.find(':');
			// std::cout << colon << std::endl;
			if ( colon != std::string::npos)
			{
				currentServer.host = value.substr(0, colon);
				currentServer.port = std::atoi(value.substr(colon + 1).c_str());
			}
			else 
			{
				currentServer.host = value; 
				currentServer.port = 80; 
			}
        }
        else if (directive.find("client_max_body_size") == 0) 
		{
            // TODO: parse and convert to int, set currentServer.client_max_body_size
			std::string value = cleanLine(directive.substr(21));
			if ( !value.empty())
				currentServer.client_max_body_size = std::atoi(value.c_str());
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
	}	
}


void	parseLocationBlock(std::istream& file, LocationConfig& current_loc)
{
	std::string line;
	while ( std::getline(file, line))
	{
		line = cleanLine(line);
		if ( line.empty())
			continue;
		if ( line == "}")
			break;
		if (line.find("root") == 0)
			current_loc.root = cleanLine(line.substr(line.find(" ") + 1));
		else if (line.find ("index") == 0)
			current_loc.index = cleanLine(line.substr(line.find(" ") + 1));
		else if ( line.find("autoindex") == 0)
			current_loc.autoindex = (line.find("on") != std::string::npos);
		else if ( line.find("upload_store") == 0)
			current_loc.upload_store = cleanLine(line.substr(line.find(" ") + 1));
		else if ( line.find("cgi_pass") == 0)
			current_loc.cgi_pass = cleanLine(line.substr(line.find(" ") + 1)); 
		else if ( line.find("allowed_methods") == 0)
		{
			std::string		methods = cleanLine(line.substr(line.find(" ") + 1));
			std::istringstream iss(methods);
			std::string 	method;
			current_loc.allowed_methods.clear();
			while ( iss >> method )
				current_loc.allowed_methods.push_back(method);
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
				// std::cout << "WE ARE IN THE SERVER BLOCK \n";
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
				// std::cout << "SERVER FOUND, WAITING FOR BRACE  \n";

				waiting_for_brace = true;
				continue ;
			}
			else if (line.find("{") == 0 && waiting_for_brace)
			{
				// std::cout << "FOUND THE '{' WE ARE IN THE SERVER BLOCK \n";

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
			// std::cout << "WE ARE IN THE SERVER LINE: " << line << std::endl;
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



				parseLocationBlock(file, current_loc);
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