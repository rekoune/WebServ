# ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <iostream>
# include <vector>
# include <map>
# include <sstream>
# include <ctime>

class Utils{
    public:
        static bool isBlank(const std::string& str);
        static bool isStartWith(std::string& str1, std::string str2);
        static void pushInVector(std::vector<char>& vec, const std::string& str);
        static std::string toString(long number);
        static std::string mapToString(std::map<std::string, std::string>& container);
        static std::string getDate();
        static std::string getFileName(std::string filePath);
        static std::string getFileType(std::map<std::string, std::string>& fileTypes, std::string fileName);
};

# endif