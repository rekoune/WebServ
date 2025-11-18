#include "../../includes/config/configStructs.hpp"

void	strToUpper(std::string& str)
{
	size_t i = 0; 
	while (i < str.size())
	{
		if ( str[i] >= 97 && str[i] <= 122)
			str[i] -= 32;
		i++;
	}
}


bool	validateMethods(std::vector<ServerConfig>::iterator serv_iter, GlobaConfig& globalConfig)
{
	for (; serv_iter != globalConfig.servers.end();
			serv_iter++ )
	{
		for (std::vector<LocationConfig>::iterator loc_iter = serv_iter->locations.begin() ;
				 loc_iter != serv_iter->locations.end(); loc_iter++)
		{
			for (std::vector<std::string>::iterator metho_iter = loc_iter->allowed_methods.begin(); 
					metho_iter != loc_iter->allowed_methods.end(); metho_iter++)
			{
				strToUpper(*metho_iter);
				if (*metho_iter != "GET"
					&& *metho_iter != "POST"
					&& *metho_iter != "PUT"
					&& *metho_iter != "DELETE"
					&& *metho_iter != "PATCH"
					&& *metho_iter != "HEAD"
					&& *metho_iter != "OPTIONS"
					&& *metho_iter != "CONNECT"
					&& *metho_iter != "TRACE")
				{
					std::cerr << "CONFIG FILE ERROR: INVALID HTTP METHOD" << std::endl;
					return false;
				}
			}
		}
	}
	return true;
}

bool	validateIpFormat(std::vector<ServerConfig>::iterator serv_iter, GlobaConfig& globalConfig)
{
	for (;serv_iter != globalConfig.servers.end(); serv_iter++)
	{

		for (std::map<std::string, std::vector<std::string> >::iterator host_iter =  serv_iter->host_port.begin(); host_iter != serv_iter->host_port.end(); host_iter++)
		{
			std::istringstream iss(host_iter->first);
			char 				dot = 'd';
			int 				num;
			int i = 0 ;
			while  (iss >> num )
			{
				dot = 0;
				iss >> dot;
				if (num < 0 || num > 255)
				{
					std::cerr << "CONFIG FILE ERROR: IP RANGE FORM 0 TO 255" << std::endl;
					return false ;
				}
					
				if ( i == 3)
					break;
				if ( dot != '.')
				{
					std::cerr << "CONFIG FILE ERROR: IP SYNTAX NOT VALID" << std::endl;
					return false;
				}
				i++;
			}
			if ( dot )
			{
					std::cerr << "CONFIG FILE ERROR: IP SYNTAX NOT VALID1" << std::endl;
					return false;
			}
 
		}
	}
	return true;
}

bool	validateLocDuplication(std::vector<ServerConfig>::iterator serv_iter, GlobaConfig& globalConfig )
{
	std::vector<LocationConfig>::iterator loc_iter = serv_iter->locations.begin();
	std::string	compared_loc_path;
	size_t 		i 	= 0;
	for (; serv_iter != globalConfig.servers.end(); serv_iter++ )
	{
		size_t 		loc_size = serv_iter->locations.size();
		i = 0;
		while (i < loc_size)
		{
			loc_iter = serv_iter->locations.begin() + i;
			compared_loc_path = (loc_iter)->path ;
			loc_iter++ ;
			while (loc_iter != serv_iter->locations.end())
			{
				if (compared_loc_path == (loc_iter)->path)
				{
					std::cerr << "CONFIG FILE ERROR: LOCATIONS DUPLACATED " << std::endl;
					return false ;
				}
				loc_iter++;
			}
			i++;
		}
	}
	return true;
	
}

bool	validateConfig(GlobaConfig& globalConfig)
{
	//Validate the methods
	std::vector<ServerConfig>::iterator serv_iter = globalConfig.servers.begin();
	if (!validateMethods(serv_iter, globalConfig))
		return false;
	//Validate the ips format
	if (!validateIpFormat(serv_iter, globalConfig))
		return false;
	serv_iter = globalConfig.servers.begin();
	if ( !validateLocDuplication(serv_iter, globalConfig))
		return false ;
	return true;
}
