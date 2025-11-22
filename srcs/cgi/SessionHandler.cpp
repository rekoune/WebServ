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


std::map<std::string, std::map< std::string, std::string> >&   SessionHandler::getData()
{
	return data;

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

	if (headers.count("cookie"))
	{
		std::map<std::string, std::string> cookies = splitCookieIntoMap(headers["cookie"]);
		if (cookies.count("SESSION_ID") /* && data.find(cookies["SESSION_ID"]) != data.end() */)
		{
			id = cookies["SESSION_ID"];
			this->is_new_client = false;
		}
		else 
			this->is_new_client = true;
	}
	else
		this->is_new_client = true;
	
	if (id.empty() && is_new_client)
	{
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
bool	isDuplacated(std::string key, std::string cookie_value)
{
	if (cookie_value.find(key) != std::string::npos)
		return true;
	return false;
}

void    SessionHandler::fetchDataToHeaders(std::map<std::string, std::string>& headers)
{
	bool			adding_semicolon = true;
	if (!headers.count("cookie"))
	{
		headers["cookie"];
		adding_semicolon = false;
	}
	std::string&	cookie_value = headers["cookie"];
	if ( cookie_value.empty())
		adding_semicolon = false;


		std::map<std::string, std::string> ses = data[current_session_id];
		for (std::map< std::string, std::string>::iterator im = ses.begin(); im != ses.end(); im++)
		{
			if (!adding_semicolon)
			{
				adding_semicolon = true;
			}
			else 
				cookie_value += "; ";
			if ((im->first == "SESSION_ID" && isSessionAvaible(cookie_value)) || isDuplacated(im->first, cookie_value))
			{
				adding_semicolon = false;
				continue ;
			}
			else 
			{
				std::string sum = im->first + "=" + im->second;
				cookie_value += sum;
			}
			
		}
}


void	SessionHandler::appendDataCookies(std::string ID, std::map<std::string, std::string>& headers_cookies)
{
	std::map<std::string, std::string>	cookie = this->data[ID];
	for (std::map<std::string, std::string>::iterator i = cookie.begin(); i != cookie.end(); i++)
	{
		if (headers_cookies.count(i->first))
		{
			continue;
		}
		else 
		{
			headers_cookies.insert(std::make_pair(i->first, i->second));
		}
	}
}

void	SessionHandler::fillDataFromHeaders(std::string ID, std::map<std::string, std::string>& headers)
{

	if (headers.empty())
		return ;
	std::string cookies = headers.find("cookie")->second;
	
	std::map<std::string, std::string> cookie_map = splitCookieIntoMap(cookies);
	appendDataCookies(ID, cookie_map);
	data[ID] = cookie_map;
}
