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

//if the file Path is a directory it returns an empty string
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
            pos = i;
            for(size_t x = i; j < _size; j++){
                if (str[x] != toContain[j])
                    break;
                x++;
            }
            if (j == _size)
                return pos;
        }
    }
    return (-1);
}

long  Utils::hexToDec(const char *hex){
    long dec;
    char * rest;
    dec = strtol(hex, &rest, 16);
    if (rest[0] != '\0')
        return -1;
    return (dec);
}


// int     singleChunk(std::vector<char> oneChunk, long sizePos, long bodyPos, std::vector<char>& body){
//     long    size;

//     std::vector<char> temp;
//     std::vector<char> tempBody;

//     Utils::pushInVector(temp, &oneChunk[0], sizePos);
//     temp.push_back('\0');
//     size = Utils::hexToDec(&temp[0]);
//     if (size == 0)
//         return (0);
//     sizePos += 2;
//     Utils::pushInVector (tempBody, &oneChunk[sizePos], bodyPos - sizePos);
//     if (size != (long)tempBody.size())
//         return -1;
//     Utils::pushInVector (body, &tempBody[0], tempBody.size());

//     return (1);
// }

// int  unchunk(){
//     // std::cout << "Oh Yeah you Get the end of the chunck" << std::endl;
//     std::vector <char> chunkBody;
//     long sizePos, bodyPos;
//     std::vector<char> tempChunk;
//     long currentPos = 0;
//     std::vector <char> body;

//     Utils::pushInVector(chunkBody, "2\r\nab\r\n3\r\ncdde\r\n0\r\n\r\n");

//     sizePos = Utils::isContainStr(&chunkBody[0], chunkBody.size(), "\r\n", 2);
//     bodyPos = Utils::isContainStr(&chunkBody[sizePos + 2], chunkBody.size() - sizePos + 2, "\r\n", 2) + 2 + sizePos;


//     Utils::pushInVector(tempChunk, &chunkBody[0], bodyPos + 2);
//     while(singleChunk(tempChunk, sizePos, bodyPos, body) == 1){
//     currentPos = bodyPos + 2;
//     sizePos = Utils::isContainStr(&chunkBody[currentPos], chunkBody.size() - currentPos, "\r\n", 2);
//     bodyPos = Utils::isContainStr(&chunkBody[currentPos + sizePos + 2], chunkBody.size() - currentPos - sizePos - 2, "\r\n", 2) + 2 + sizePos;
//     tempChunk.clear();
//     Utils::pushInVector(tempChunk, &chunkBody[currentPos], (bodyPos + 2));
//     }
//     return (0);
// }


// int main (){
//     std::cout <<  unchunk() << std::endl ;
// }
