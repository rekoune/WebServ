#include "../../includes/cgi/SessionHandler.hpp"


SessionHandler::SessionHandler()
{
	this->is_new_client = false;
	static bool seeded = false;
	if (!seeded)
	{

		std::srand(std::time(NULL) );
		seeded = true;
	}

}

SessionHandler::SessionHandler(const SessionHandler& other)
{
	this->data = other.data;
}

SessionHandler::~SessionHandler()
{

}

SessionHandler& SessionHandler::operator=(const SessionHandler& other)
{
	if (this == &other)
		return (*this);
	this->data = other.data;
	return (*this);
}


std::string SessionHandler::generateID()
{
	std::string chars("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
	std::string ID;
	for (int i = 0; i < 16; i++)
	{
		char char_var = chars[rand() % 62];
		ID += char_var;
	}
	return ID;

}


void SessionHandler::addSession(std::map<std::string, std::string>& headers)
{
	std::string id;

	// CHECK IF CLIENT SENT SESSION ID , LIKE ITS AN OLD CLIENT THAT ALREADY HAS A SESSION, JUST NEEDS TO UPDATE
	if (headers.count("Cookie"))
	{
		std::map<std::string, std::string> cookies = splitCookieIntoMap(headers["Cookie"]);
		// std::map<std::string, std::map<std::string, std::string> >::iterator i = data.begin();
		if (cookies.count("SESSION_ID") && data.find(cookies["SESSION_ID"]) != data.end())
		{
			id = cookies["SESSION_ID"];
			this->is_new_client = false;
		}
		else 
			this->is_new_client = true;
	}
	else
		this->is_new_client = true;
	
	std::cout << "id:" << id << std::endl;
	if (id.empty() && is_new_client)
	{
		// MEANS NEW CLIENT: NEW ID , SET-COOKIE, SET SESSION_ID
		// GENREATE ID 
		std::cout << " GENERATE NEW ID =========\n";
		id = generateID();
		this->is_new_client = true;
		headers.insert(std::make_pair("Set-Cookie",  "SESSION_ID=" + id + "; Path=/"));
		if (!headers["Cookie"].empty())
		{
			headers["Cookie"] +=  "; SESSION_ID=" + id ;
		}
		else 
			headers["Cookie"] +=  "SESSION_ID=" + id ;

	}
	current_session_id = id;
	if (headers.empty())
		return ;

	fillDataFromHeaders(current_session_id, headers);
}

bool	isSessionAvaible(std::string cookie_value_from_headers)
{
	if (cookie_value_from_headers.find("SESSION_ID=") != std::string::npos)
		return true;
	return false;
}

void    SessionHandler::fetchDataToHeaders(std::map<std::string, std::string>& headers)
{

	std::string&	cookie_value = headers["Cookie"];
	bool			adding_semicolon = true;
	if ( cookie_value.empty())
		adding_semicolon = false;

	std::cout << "cookie_value:" << cookie_value << std::endl;
	for (std::map<std::string, std::map< std::string, std::string> >::iterator i = this->data.begin(); i != this->data.end(); i++)
	{

		std::cout << "sission :" << i->first << std::endl << "COOOKIES" << std::endl;
		for (std::map< std::string, std::string>::iterator im = i->second.begin(); im != i->second.end(); im++)
		{
			if (!adding_semicolon)
			{
				adding_semicolon = true;
			}
			else 
				cookie_value += "; ";
			if (im->first == "SESSION_ID" && isSessionAvaible(cookie_value))
				continue ;
			cookie_value += (im->first + "=" + im->second);
			std::cout << "after adding" << cookie_value << std::endl;
		}
	}
}


// void	SessionHandler::appendHeaders(std::map<std::string, std::string> headers)
// {
//     (void)headers;
// }




void    SessionHandler::printSessionData()
{
	std::cout << "PRINTING SESSION DATA" << std::endl;
	for (std::map<std::string, std::map< std::string, std::string> >::iterator i = this->data.begin(); i != this->data.end(); i++)
	{
		std::cout << "sission :" << i->first << std::endl;
		for (std::map< std::string, std::string>::iterator im = i->second.begin(); im != i->second.end(); im++)
		{
			std::cout << im->first << ":" << im->second << std::endl ;
		}
	}
	std::cout << "PRINTING SESSION DATA -FINISHED-" << std::endl;
}

void	SessionHandler::appendDataCookies(std::string ID, std::map<std::string, std::string>& headers_cookies)
{
	std::map<std::string, std::string>	cookie = this->data[ID];
	for (std::map<std::string, std::string>::iterator i = cookie.begin(); i != cookie.end(); i++)
	{
		if (headers_cookies.count(i->first))
			continue;
		else 
		{
			headers_cookies.insert(std::make_pair(i->first, i->second));
		}
	}
}

void	SessionHandler::fillDataFromHeaders(std::string ID, std::map<std::string, std::string>& headers)
{
	(void)headers;

	if (headers.empty())
		return ;
	std::string cookies = headers.find("Cookie")->second;
	// split the cookie 
	std::map<std::string, std::string> cookie_map = splitCookieIntoMap(cookies);
	appendDataCookies(ID, cookie_map);


	data[ID] = cookie_map;
}
