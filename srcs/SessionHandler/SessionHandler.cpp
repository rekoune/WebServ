#include "../../includes/cgi/SessionHandler.hpp"


SessionHandler::SessionHandler()
{}

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
    return (*this)
}

void SessionHandler::addSession()
{
    // GENREATE ID 
    // PARSE COOKIES INTO SESSION_DATA
    // 
}


void	SessionHandler::fetchData()
{

}


void	SessionHandler::appendHeaders(std::map<std::string, std::string> headers)
{

}


void	SessionHandler::fillData(std::map<std::string, std::string> headers)
{

}
