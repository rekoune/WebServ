# ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <iostream>
# include <vector>
# include <map>
# include <sstream>
# include <ctime>
# include <cstdlib>
# include <unistd.h>
# include <fstream>

class Utils{
    public:
        static bool         isBlank(const std::string& str);
        static bool         isStartWith(std::string& str1, std::string str2);
        static void         pushInVector(std::vector<char>& vec, const std::string str);
        static void         pushInVector(std::vector<char>& vec, const char* str, size_t size);
        static std::string  toString(long number);
        static std::string  mapToString(std::map<std::string, std::string>& container);
        static std::string  getDate();
        static std::string  getFileName(std::string filePath);
        static std::string  getFileType(std::map<std::string, std::string>& fileTypes, std::string fileName);
        static std::string  findExtensionByMime(std::map<std::string, std::string>& fileTypes, std::string mime);
        static long         isContainStr(const char *str, size_t strSize, const char *toContain, size_t _size);
        static long         hexToDec(const char* hex);
        static void         strToLower (std::string& str);
        static bool         isReadable(std::string& filePath);
        static std::string  randomName(std::string& prefix);
        static void         trimSpaces(std::string& str);
        static size_t       strToNumber(std::string& str);
        static size_t       getFileSize(const std::string& filePath);
        static bool            isScript(std::string& path, std::map<std::string, std::string>& cgiExtentions);

};

# endif