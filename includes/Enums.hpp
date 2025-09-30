#ifndef ENUMS_HPP
#define ENUMS_HPP

enum HttpStatusCode {
    CONTINUE = 100, 

    OK = 200,
    CREATED = 201,
    NO_CONTENT = 204,
    PARTIAL_CONTENT = 206,

    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    CONTENT_TOO_LARGE = 413,

    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    HTTP_VERSION_NOT_SUPPORTED = 505
};

enum ParseState {
    PARSE_START,       
    PARSE_HEADERS,     
    PARSE_BODY_LENGTH, 
    PARSE_BODY_CHUNKED,
    PARSE_COMPLETE,    
    PARSE_ERROR        
};

enum PathTypes{
    F,
    SCRIPT,
    DIR_LS,
};

# endif

