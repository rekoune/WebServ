#include "../../../includes/GetHandler.hpp"

GetHandler::GetHandler(): done(false){}

GetHandler::GetHandler(const GetHandler& other){
    *this = other;
}
GetHandler::GetHandler(std::string path, size_t position){
    this->path = path;
    this->position = position;
    this->fileSize = Utils::getFileSize(path);
    this->File.open(path.c_str(), std::ios::in | std::ios::binary);
    done = false;
}
GetHandler& GetHandler::operator=(const GetHandler& other){
    this-> done = other.done;
    this-> path = other.path;
    this-> fileSize = other.fileSize;
    this-> position = other.position;
    return (*this);
}

void GetHandler::setPath(std::string path){
    this->path = path;
    done = false;
    this->fileSize = Utils::getFileSize(path);
    if (File.is_open())
        File.close();
    File.open(path.c_str(), std::ios::in | std::ios::binary);
}
void GetHandler::setPosition(size_t position){
    this->position = position;
    done = false;
}

bool GetHandler::isDone(){
    return (this->done);
}

std::vector<char> GetHandler::get(size_t size){

    if (!File.is_open()){
        done = true;
        return (std::vector<char>());
    }
    if (position >= fileSize){
      position = 0;
      done = true;
      File.close();
      return std::vector<char>();  
    }
        
    if (size + position > fileSize){
        size = fileSize - position;
    }
    File.seekg(position, std::ios::beg);
    std::vector<char> body(size);
    File.read(&body[0], size);
    position += size;
    if (position >= fileSize - 1){
        done = true;
        File.close();
    }
    return body;
}