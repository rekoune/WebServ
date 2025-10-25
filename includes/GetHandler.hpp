# ifndef GETHANDLER_HPP
# define GETHANDLER_HPP


# include "Structs.hpp"
# include "Utils.hpp"
# include <vector>
# include <fstream>


class GetHandler{
    private:
        bool done;
        size_t position;
        std::string path;
        size_t fileSize;
    public:
        std::ifstream File;

        GetHandler();
        GetHandler(std::string path, size_t position);
        GetHandler(const GetHandler& other);
        void setPath(std::string path);
        void setPosition(size_t position);
        GetHandler& operator=(const GetHandler& other);
        std::vector<char> get(size_t size);
        bool isDone();

};

# endif
