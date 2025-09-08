# ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <iostream>
# include <vector>
# include <map>
# include <sstream>

class Utils{
    public:
        static bool isBlank(const std::string& str);
        static bool isStartWith(std::string& str1, std::string str2);
        static void pushInVector(std::vector<char>& vec, const std::string& str);
        static std::string toString(long number);
        static std::string mapToString(std::map<std::string, std::string>& container);
};

# endif