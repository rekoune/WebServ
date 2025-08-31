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
    
}