#include "../../includes/cgi/SessionHandler.hpp"


SessionHandler::SessionHandler()
{
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
        if (cookies.count("SESSION_ID") && data.find(cookies["SESSION_ID"]) != data.end())
        {
            id = cookies["SESSION_ID"];
        }
    }
    
    if (id.empty())
    {
        // MEANS NEW CLIENT: NEW ID , SET-COOKIE, SET SESSION_ID
    // GENREATE ID 
    std::cout << " GENERATE NEW ID =========\n";
        id = generateID();
        headers.insert(std::make_pair("Set-Cookie",  "SESSION_ID=" + id /* + "; Path=/; HttpOnly" */));
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

    fillDataFromHeaders(id, headers);
}


std::map<std::string, std::string>	SessionHandler::fetchDataToHeaders()
{
    std::map<std::string, std::string>  headers;
    for (std::map<std::string, std::map< std::string, std::string> >::iterator i = this->data.begin(); i != this->data.end(); i++)
    {
        // std::cout << "sission :" << i->first << std::endl << "COOOKIES" << std::endl;
        for (std::map< std::string, std::string>::iterator im = i->second.begin(); im != i->second.end(); im++)
        {
            // std::cout << im->first << ":" << im->second << std::endl ;
            headers.insert(std::make_pair(im->first, im->second));
        }
    }
    return headers;

}


void	SessionHandler::appendHeaders(std::map<std::string, std::string> headers)
{
    (void)headers;
}




void    SessionHandler::printSessionData()
{
    for (std::map<std::string, std::map< std::string, std::string> >::iterator i = this->data.begin(); i != this->data.end(); i++)
    {
        std::cout << "sission :" << i->first << std::endl << "COOOKIES" << std::endl;
        for (std::map< std::string, std::string>::iterator im = i->second.begin(); im != i->second.end(); im++)
        {
            std::cout << im->first << ":" << im->second << std::endl ;
        }
    }
}


void	SessionHandler::fillDataFromHeaders(std::string ID, std::map<std::string, std::string>& headers)
{
    (void)headers;

    if (headers.empty())
        return ;
    std::string cookies = headers.find("Cookie")->second;
    // std::cout << "cookie->: " << cookies << std::endl;
    // split the cookie 
    std::map<std::string, std::string> cookie_map = splitCookieIntoMap(cookies);

    data[ID] = cookie_map;
    // printMapStr(data[ID]);
    // this->printSessionData();

}
