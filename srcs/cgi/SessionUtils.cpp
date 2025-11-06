#include "../../includes/cgi/SessionHandler.hpp"




bool isitspaceUtil(char c)
{
	if (c == 32 || (c >= 9 && c <= 13))
		return true;
	return false;
}

std::string cleanLineUtil(std::string line)
{
    size_t  hashpos = line.find("#");
    if  (hashpos != std::string::npos)
        line.erase(hashpos);
    
    size_t	start = 0;
	while (start < line.size() && isitspaceUtil(line[start]) )
		start++;
	
	size_t	end = line.size();
	while (end > 0 && isitspaceUtil(line[end - 1]))
		end--;
	line = line.substr(start, end - start);
	return (line);
}


void printMapStr(std::map<std::string, std::string> map)
{
    std::cout << "printing map of string string \n";
    for (std::map<std::string, std::string>::iterator i = map.begin(); i != map.end(); i++)
    {
        std::cout << "i->first :" << i->first << std::endl;
        std::cout << "i->second :" << i->second << std::endl;
    }

}


void        parseCookieDirective(std::map<std::string, std::string>& map, std::string cookie_directive)
{
    std::string name; 
    std::string value;
    std::istringstream iss (cookie_directive);

    std::getline(iss, name, '=');
    name  = cleanLineUtil(name);
    // std::cout << "name:" << name << std::endl;

    std::getline(iss, value);
    value  = cleanLineUtil(value);
    // std::cout << "value:" << value << std::endl;

    map[name] = value;
}

std::map<std::string, std::string>  splitCookieIntoMap(std::string cookie)
{
    std::map<std::string, std::string>  map;
    std::string     cookie_directive;


    size_t          pos = 0;
    size_t          semic_pos = 0;
    while (((semic_pos = cookie.find(";", pos)) != std::string::npos))
    {
        cookie_directive = cookie.substr(pos, semic_pos - pos);
        pos = semic_pos + 1;
        // std::cout << "cookie_directive:" << cookie_directive << std::endl;
        parseCookieDirective(map, cookie_directive);

    }
    if ( pos < cookie.size())
    {
        cookie_directive = cookie.substr(pos, cookie.size() - pos);
        // std::cout << "cookie_directive:" << cookie_directive << std::endl;
        parseCookieDirective(map, cookie_directive);

    }
    return map; 
}

