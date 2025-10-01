# ifndef UPLOADHANDLER_HPP
# define UPLOADHANDLER_HPP

# include "Enums.hpp"
# include "Structs.hpp"
# include <vector>
# include <map>
# include <fstream>
# include <unistd.h>
# include <sys/stat.h>
# include "Utils.hpp"

class UploadHandler{
    private:
        enum MultipartState {
            SEARCHING_BOUNDARY,
            SEARCHING_HEADERS,
            SEARCHING_BODY,
            END
        };
    
        MultipartState      currentState;
        HttpResourceInfo    resInfo;
        std::vector<char>   bodySaver;
        std::ofstream       bodyFile;
        std::string         uploadPath;
        ParseState          parseState;
        std::string         contentType;
        std::string         boundary;
        std::vector<std::string> openedFiles;
        std::map<std::string, std::string>  fileTypes;
        long long           uploadSize;

        void                setFileTypes();
        void                setFullPathByType(std::string& path, PathTypes& pathType, std::string contentType);
        HttpStatusCode      getPathType(std::string path, PathTypes& type);
        HttpStatusCode      getUploadPath(std::string& uploadPath);
        HttpStatusCode      chunkedBodyHandling(const char* data, size_t size);
        ParseState          singleChunk(std::vector<char>& oneChunk, size_t size);
        HttpStatusCode      contentLengthHandling(const char* data, size_t size);
        HttpStatusCode      handleByContentType(const char* data, size_t size);
        HttpStatusCode      extractHeaders(std::string bodyHeaders);
        HttpStatusCode      checkHeaders(std::map<std::string, std::string>& headers);
        HttpStatusCode      multipartHandling(const char* data, size_t size);
        HttpStatusCode      searchForBoundary();
        HttpStatusCode      searchForHeaders();
        HttpStatusCode      searchForBody();
        void                clearFiles(std::vector<std::string>& files);

    public:
        UploadHandler();
        ~UploadHandler();
        UploadHandler(HttpResourceInfo& resInfo);
        UploadHandler(const UploadHandler& other);
        UploadHandler& operator=(const UploadHandler& other);
        void            setResInfo(const HttpResourceInfo& resInfo);
        void            setParseState(const ParseState& parseState);
        void            setUploadSize(const long long& uploadSize);
        HttpResourceInfo    getResourseInfo( void ) const;

        ParseState          upload(const char *data, size_t size);
    
};

# endif