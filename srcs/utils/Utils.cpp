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

std::string Utils::getDate(){
    std::time_t now;
    std::tm localTime;
    char    date[100];

    now = std::time(NULL);
    localTime = *std::localtime(&now);
    
    std::strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", &localTime);
    std::string strDate(date);
    return (strDate);
}

std::string Utils::getFileType(std::map<std::string, std::string>& fileTypes, std::string fileName){
    size_t found = fileName.find(".");
    std::map<std::string, std::string>::iterator typeIt;
    if (found != std::string::npos){
        found++;
        typeIt = fileTypes.find(&fileName[found]);
        if (typeIt != fileTypes.end()){
            return (typeIt->second);
        }
    }
    return ("text/plain");
}

//if the file Path is a directory it returns an empty string
std::string Utils::getFileName(std::string filePath){
    std::stringstream ss(filePath);
    std::string fileName;

    getline(ss, fileName, '/');
    while(!ss.eof())
        getline(ss, fileName, '/');
    return (fileName);
}
