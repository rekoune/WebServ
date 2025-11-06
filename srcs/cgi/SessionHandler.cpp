#include "../../includes/cgi/SessionHandler.hpp"


SessionHandler::SessionHandler()
{
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
    std::srand(std::time(NULL));
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
        std::map<std::string, std::string> cookies = splitCookieIntoMap(headers["cookie"]);
        if (cookies.count("SESSION_ID"))
        {
            id = cookies["SESSION_ID"];
        }
    }
    
    if (id.empty())
    {
        // MEANS NEW CLIENT: NEW ID , SET-COOKIE, SET SESSION_ID
    // GENREATE ID 
        id = generateID();
        headers["Set-Cookie"] += "; SESSION_ID=" + id;
        headers["Cookie"] +=  "; SESSION_ID=" + id ;
    }

    // std::cout << "ID: " << id << std::endl;

    current_session_id = id;
    if (headers.empty())
        return ;
        
    fillDataFromHeaders(id, headers);
}


void	SessionHandler::fetchDataToHeaders()
{


}


void	SessionHandler::appendHeaders(std::map<std::string, std::string> headers)
{
    (void)headers;
}




void    SessionHandler::printSessionData()
{
    for (std::map<std::string, std::map< std::string, std::string> >::iterator i = data.begin(); i != data.end(); i++)
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
    this->printSessionData();
}
