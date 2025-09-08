# include "../../includes/Utils.hpp"

bool Utils::isBlank (const std::string& str){
    for(size_t i = 0; i < str.size(); i++){
        if (str.at(i) != ' ')
            return (false);
    }
    return (true);
}

bool Utils::isStartWith(std::string& str1, std::string str2){
    if (str1 == str2)
        return (true);
    size_t i = 0;
    for(; i < str1.length(); i++){
        if (i >= str2.length()){
            if (str1.at(i) != '/' && str2.length() > 1)
                return (false);
            return (true);
        }
        if (str1.at(i) != str2.at(i))
            return (false);
    }
    if(i < str2.length())
        return (false);
    return (true);
}

void Utils::pushInVector(std::vector<char>& vec, const std::string& str){
    for(size_t i = 0; i < str.length(); i++){
        vec.push_back(str.at(i));
    }
}

std::string Utils::toString(long number){
    std::stringstream ss;

    ss << number;
    return (ss.str());
}

std::string Utils::mapToString(std::map<std::string, std::string>& container){
    std::string str;
    std::map<std::string, std::string>::iterator it;

    it = container.begin();
    for(; it != container.end(); it++){
        str.append(it->first);
        str.append(": ");
        str.append(it->second);
        str.append("\n");
    }
    if (!str.empty()){
        str.erase(str.length() - 1);
        str.append("\r\n\r\n");
    }
    return (str);
}