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
	if ( line.find("root") == 0)
	{
		currentServer.root = cleanLine(line.substr(line.find(" ")));
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
		std::cout << line << std::endl; 	// Print for debugging 
		if (!in_server_block)
		{
			if ((line.find("server") == 0 && line.find("{") != std::string::npos))
			{
				std::cout << "WE ARE IN THE SERVER BLOCK \n";
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
						configStructInit(line, currentserver);
				}
				continue ;
			}
			else if ( line.find("server") == 0 &&  line.find("{") == std::string::npos)
			{
				std::cout << "SERVER FOUND, WAITING FOR BRACE  \n";

				waiting_for_brace = true;
				continue ;
			}
			else if (line.find("{") == 0 && waiting_for_brace)
			{
				std::cout << "FOUND THE '{' WE ARE IN THE SERVER BLOCK \n";

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
			std::cout << "WE ARE IN THE SERVER LINE: " << line << std::endl;
			if ( line == "}")
			{
				globalConfig.servers.push_back(currentserver);
				in_server_block = false;
			}
			else 
				configStructInit(line, currentserver);
		}

		

		
    }
	if (waiting_for_brace)
		std::cerr << "ERROR : NO SERVER BLOCK\n";
    return globalConfig;
}