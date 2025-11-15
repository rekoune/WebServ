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

	// CHECK IF CLIENT SENT SESSION ID , LIKE ITS AN OLD CLIENT THAT ALREADY HAS A SESSION, JUST NEEDS TO UPDATE
	if (headers.count("cookie"))
	{
		std::cerr << "(headers.count('Cookie')" << std::endl;
		std::map<std::string, std::string> cookies = splitCookieIntoMap(headers["cookie"]);
		// std::map<std::string, std::map<std::string, std::string> >::iterator i = data.begin();
		if (cookies.count("SESSION_ID") /* && data.find(cookies["SESSION_ID"]) != data.end() */)
		{
			id = cookies["SESSION_ID"];
			std::cerr << "================================" << id << "======================================" << std::endl; 
			this->is_new_client = false;
		}
		else 
			this->is_new_client = true;
	}
	else
		this->is_new_client = true;
	
	std::cerr << "id:" << id << std::endl;
	if (id.empty() && is_new_client)
	{
		// MEANS NEW CLIENT: NEW ID , SET-COOKIE, SET SESSION_ID
		// GENREATE ID 
		std::cerr << " GENERATE NEW ID =========\n";
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

	// std::cerr << "\033[32mcookie_value:\033[0m" << cookie_value << std::endl;

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
				// std::cerr << "\033[32m IN CONTINUE CONDITION \033[0m" << std::endl;
				adding_semicolon = false;
				continue ;
			}
			else 
			{
				std::string sum = im->first + "=" + im->second;
				cookie_value += sum;
			}
			// std::cerr << "\033[32m PRINTING THE FIRST AND THE SECOND\033[0m" << im->first << im->second << std::endl;
			
		}
	// std::cerr << "\033[32mcookie_value:after adding\033[0m" << cookie_value << std::endl;
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
		{
			std::cerr << "found a alredy cookie key exists " << i->first << std::endl;
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
	(void)headers;

	if (headers.empty())
		return ;
	std::string cookies = headers.find("cookie")->second;
	std::cerr <<  "\033[0m" << "coookies: " << cookies << std::endl;
	// split the cookie 
	std::map<std::string, std::string> cookie_map = splitCookieIntoMap(cookies);
	appendDataCookies(ID, cookie_map);

	std::cerr << " \033[35m PRINTING THE SESSION WITH ID IN FILLDATAFORMHEADERS \033[0m" << std::endl;
	printMapStr(data[ID]);
	data[ID] = cookie_map;
}


// www/task-manager/
// ├── index.html
// ├── style.css
// ├── app.js
// └── cgi-bin/
//     ├── common.py
//     ├── api_tasks.py
//     └── api_session.py