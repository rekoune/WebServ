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
        static long         isContainStr(const char *str, size_t strSize, const char *toContain, size_t _size);
        static long         hexToDec(const char* hex);
        static void         strToLower (std::string& str);
        static bool         isReadable(std::string& filePath);
};

# endif