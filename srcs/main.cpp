#include "../includes/config/configStructs.hpp"

int main(int ac, char **av)
{
    (void)ac;
    GlobaConfig globalconfig;
    std::cout << " =========== main ==================\n";
    if ( ac == 2)
        globalconfig = parseConfig(av[1]);
    else 
        std::cout << " Error : no config file given \n";
    //PRININ SERVERS IN COFIG 
    std::vector<ServerConfig>::iterator  iter = globalconfig.servers.begin();
    if ( iter == globalconfig.servers.end())
        std::cout << " the servers vector is empty\n";
    else 
    {

        std::cout << iter->root << std::endl;
        std::cout << iter->client_max_body_size << std::endl;
        std::cout << iter->root << std::endl;
    }
    
}