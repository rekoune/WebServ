# ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include <sstream>
# include <exception>

enum Method{
    GET,
    POST,
    DELETE
};

class Request{
    private:
        Request();
        std::string req;
        Method method;
        std::string target;
        std::string HttpVersion;
    public:
        Request(std::string req);
        ~Request();
        Request( const Request& other);
        Request& operator=( const Request& other);
        class BadRequest;
        void parseRequest();
        void parseRequestLine(std::string& reqLine);
        void setMethod(std::string& method);
        void setTarget(std::string& target);
        void setHttpVersion(std::string& httpVersion);

};

class Request::BadRequest : public std::exception {
    public:
    const char* what() const throw();
};

# endif