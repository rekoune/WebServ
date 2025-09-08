# include "../../../includes/Response.hpp"

Response::Response(){};
Response::~Response(){};
Response::Response(const Response& other){
    *this = other;
}
Response& Response::operator=(const Response& other){
    this->resInfo = other.resInfo;
    this->resElements = other.resElements;
    this->response = other.response;
    return (*this);
}
Response::Response(const HttpResponseInfo info){
    this->resInfo = info;
}
void Response::setResInfo(const HttpResponseInfo& info){
    this->resInfo = info;
}
std::vector<char> Response::getResponse () const {
    return (this->response);
}

void Response::generateStatusLine(std::string& str, long status, std::string message){
    str.append(Utils::toString(status));
    str.append(" ");
    str.append(message);
    str.append("\r\n");
}

std::string Response::getStatusLine(){
    std::string statusLine;

    statusLine.append("HTTP/1.1 ");
    switch (resInfo.status)
    {
        case CONTINUE:
            generateStatusLine(statusLine, CONTINUE, "CONTINUE");
            break;
        case OK:
            generateStatusLine(statusLine, OK, "OK");
            break;
        case CREATED:
            generateStatusLine(statusLine, CREATED, "CREATED");
            break;
        case NO_CONTENT:
            generateStatusLine(statusLine, NO_CONTENT, "NO_CONTENT");
            break;
        case BAD_REQUEST:
            generateStatusLine(statusLine, BAD_REQUEST, "BAD_REQUEST");
            break;
        case UNAUTHORIZED:
            generateStatusLine(statusLine, UNAUTHORIZED, "UNAUTHORIZED");
            break;
        case FORBIDDEN:
            generateStatusLine(statusLine, FORBIDDEN, "FORBIDDEN");
            break;
        case NOT_FOUND:
            generateStatusLine(statusLine, NOT_FOUND, "NOT_FOUND");
            break;
        case METHOD_NOT_ALLOWED:
            generateStatusLine(statusLine, METHOD_NOT_ALLOWED, "METHOD_NOT_ALLOWED");
            break;
        case CONTENT_TOO_LARGE:
            generateStatusLine(statusLine, CONTENT_TOO_LARGE, "CONTENT_TOO_LARGE");
            break;
        case INTERNAL_SERVER_ERROR:
            generateStatusLine(statusLine, INTERNAL_SERVER_ERROR, "INTERNAL_SERVER_ERROR");
            break;
        case NOT_IMPLEMENTED:
            generateStatusLine(statusLine, NOT_IMPLEMENTED, "NOT_IMPLEMENTED");
            break;
        case HTTP_VERSION_NOT_SUPPORTED:
            generateStatusLine(statusLine, HTTP_VERSION_NOT_SUPPORTED, "HTTP_VERSION_NOT_SUPPORTED");
            break;
    }
    return (statusLine);
}

std::string         Response::getStatusMessage(HttpStatusCode status){
    switch (status)
    {
        case CONTINUE:
            return ("CONTINUE");
        case OK:
            return ("OK");
        case CREATED:
            return ("CREATED");
        case NO_CONTENT:
            return ("NO_CONTENT");
        case BAD_REQUEST:
            return ("BAD_REQUEST");
        case UNAUTHORIZED:
            return ("UNAUTHORIZED");
        case FORBIDDEN:
            return ("FORBIDDEN");
        case NOT_FOUND:
            return ("NOT_FOUND");
        case METHOD_NOT_ALLOWED:
            return ("METHOD_NOT_ALLOWED");
        case CONTENT_TOO_LARGE:
            return ("CONTENT_TOO_LARGE");
        case INTERNAL_SERVER_ERROR:
            return ("INTERNAL_SERVER_ERROR");
        case NOT_IMPLEMENTED:
            return ("NOT_IMPLEMENTED");
        case HTTP_VERSION_NOT_SUPPORTED:
            return ("HTTP_VERSION_NOT_SUPPORTED");
    }
    return "DEFAUTL";
}

std::vector<char>  Response::generateErrorBody(){
    std::stringstream bodyStream;
    std::vector<char> body;

    bodyStream << "<html>\n<head><title>";
    bodyStream << Utils::toString(this->resInfo.status);
    bodyStream << " ";
    bodyStream << getStatusMessage(this->resInfo.status);
    bodyStream << "</title></head>\n<body>\n<center><h1>";
    bodyStream << Utils::toString(this->resInfo.status);
    bodyStream << " ";
    bodyStream << getStatusMessage(this->resInfo.status);
    bodyStream << "</h1></center>\n<hr><center>WebServer</center>\n</body>\n</html>\n";
    Utils::pushInVector(body, bodyStream.str());
    return (body);
}

std::map<std::string, std::string>  Response::generateErrorHeaders(){
    std::map<std::string, std::string> headers;

    headers.insert(std::pair<std::string, std::string> ("Server", "WebServer"));
    headers.insert(std::pair<std::string, std::string> ("Date", "Mon, 08 Sep 2025 14:39:23 GMT"));
    headers.insert(std::pair<std::string, std::string> ("Content-Type", "text/html"));
    headers.insert(std::pair<std::string, std::string> ("Content-Length", Utils::toString(resElements.body.size())));
    headers.insert(std::pair<std::string, std::string> ("Connection", "keep-alive"));

    return (headers);
}

void    Response::errorHandling(){
    resElements.statusLine = getStatusLine();
    resElements.body = generateErrorBody();
    resElements.headers = generateErrorHeaders();

    Utils::pushInVector(response, resElements.statusLine);
    Utils::pushInVector(response, Utils::mapToString(resElements.headers));
    response.insert(response.end(), resElements.body.begin(), resElements.body.end());

}

void Response::handel(){
    if (resInfo.status != OK){
        errorHandling();
    }
}

