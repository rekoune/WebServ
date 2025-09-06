# ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <iostream>

class Utils{
    public:
        static bool isBlank(const std::string& str);
        static bool isStartWith(std::string& str1, std::string str2);
};

# endif