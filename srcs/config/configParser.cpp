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
	while (end > 0 && isspace(line[end - 1]))
		end--;
	line = line.substr(start, end - start);
	return (line);
}
void	configInit(GlobaConfig& globalconfig)
{
	(void)globalconfig;
	//filling the struct of server 
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
    while (std::getline(file, line))
    {
		line = cleanLine(line);     //	triming and removing comments 
		if (line.empty())           //  checking if the line is empty
			continue;
		std::cout << line << std::endl; 	// Print for debugging 
		// if ((line.find("server") == 0 && line.find("{") == std::string::npos) || server_bracket_open) //check "server" and "{"

		// so there is 3 states in wich the first line would be in  
		// 1 and the easiest the server and { in the same line so we IN_SERVER_BLOCK
		// 2 server so we waiting for BRACE WAITING_FOR_BRACE 
		// 3 FOUND THE BRACE WE CHECK AFTER IT AND CONTINUE READIN  NEXT LINE
		if (!in_server_block)
		{
			if ((line.find("server") == 0 && line.find("{") != std::string::npos))
			{
				std::cout << "WE ARE IN THE SERVER BLOCK \n";
				//idk if i need to set the flags to true or no TO SEE
				in_server_block = true;
				//SKIP the {
				size_t		brace_pos = line.find("{");
				std::string	remaining = line.substr(brace_pos + 1);
				remaining = cleanLine(remaining);
				if (!remaining.empty())
				{
					if ( remaining  == "}")
						in_server_block = false;
					else 
						configInit(globalConfig);
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
						in_server_block = false;
					else 
						configInit(globalConfig);
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
			std::cout << "WE ARE IN THE SERVER LINE: " << line << std::endl;

		
    }
    return globalConfig;
}