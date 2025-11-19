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

void Utils::pushInVector(std::vector<char>& vec, const std::string str){
    for(size_t i = 0; i < str.length(); i++){
        vec.push_back(str.at(i));
    }
}
void Utils::pushInVector(std::vector<char>& vec, const char* str, size_t size){
    for(size_t i = 0; i < size; i++){
        vec.push_back(str[i]);
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

std::string Utils::getFileName(std::string filePath){
    std::stringstream ss(filePath);
    std::string fileName;

    getline(ss, fileName, '/');
    while(!ss.eof())
        getline(ss, fileName, '/');
    return (fileName);
}

long Utils::isContainStr(const char *str, size_t strSize, const char *toContain, size_t _size){
    long pos = -1;
    for(size_t i = 0; i < strSize; i++){
        size_t  j = 0;
        if (str[i] == toContain[j]){
            for(size_t x = i; j < _size; j++){
                if (x >= strSize || str[x] != toContain[j])
                    break;
                x++;
            }
            if (j == _size){
                pos = i;
                break;
            }
        }
    }
    return (pos);
}

long  Utils::hexToDec(const char *hex){
    long dec;
    char * rest;
    dec = strtol(hex, &rest, 16);
    if (rest[0] != '\0')
        return -1;
    return (dec);
}

void    Utils::strToLower (std::string& str){
    for(size_t i = 0; i < str.length(); i++){
        str.at(i) = std::tolower(str.at(i));
    }
}

bool Utils::isReadable(std::string& filePath){
    return (access(filePath.c_str(), R_OK) == 0);
}


std::string  Utils::findExtensionByMime(std::map<std::string, std::string>& fileTypes, std::string mime){
    std::string extension(".");
    std::map<std::string, std::string>::iterator it;

    it = fileTypes.begin();
    for(; it != fileTypes.end(); it++){
        if (it->second == mime){
            extension.append(it->first);
            return (extension);
        }
    }
    extension.append("bin");
    return (extension);
}

std::string  Utils::randomName(std::string& prefix){
    std::stringstream ss;

    ss << prefix;
    ss << std::rand();
    return (ss.str());
}

void Utils::trimSpaces(std::string& str){
    size_t start;
    long  end;

    start = 0;
    end = (long)str.length();
    while(start < str.length() && str[start] == ' ')
        start++;
    while(end >= 0 && str[end] == ' ')
        end--;
    str = std::string(str.begin() + start, str.begin() + end);
}

size_t       Utils::strToNumber(std::string& str){
    size_t nb;
    std::stringstream ss(str);

    ss >> nb;
    return (nb);
}

size_t       Utils::getFileSize(const std::string& filePath){
    size_t size = 0;
    std::ifstream file(filePath.c_str(), std::ios::binary | std::ios::ate); 
    if (!file)
    {
        std::cout << "file fail to open" << std::endl;
    }
    size = file.tellg();
    file.close();
    return (size);
}

bool Utils::isScript(std::string& path, std::vector<std::string>& cgiExtentions){
    std::string extention;
    std::string fileName;
    size_t      dotPos;

    fileName = Utils::getFileName(path);
    dotPos = fileName.find(".");
    if (dotPos != std::string::npos){
        extention.append(fileName.begin() + dotPos , fileName.end());
        for (size_t i = 0; i < cgiExtentions.size(); i++)
        {
            if (cgiExtentions.at(i) == extention)
                return (true);
        }
    }
    return false;
}

std::vector<char>    Utils::readFile(std::string& filePath){
    std::ifstream file;
    size_t size = 0;

    file.open(filePath.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open())
        return (std::vector<char>());
    size = Utils::getFileSize(filePath);
    std::vector<char> body(size);
    file.read(&body[0], size);
    file.close();
    return (body);
}

void    Utils::decodeUrl(std::string& url, std::string encodedCharacter, std::string toReplaceWith){
    int pos;
    pos = Utils::isContainStr(&url[0], url.length(), &encodedCharacter[0], encodedCharacter.length());

    for(; pos != -1; ){
        url.erase(pos, encodedCharacter.length());
        url.insert(pos, toReplaceWith);
        pos = Utils::isContainStr(&url[0], url.length(), &encodedCharacter[0], encodedCharacter.length());
    }
}

