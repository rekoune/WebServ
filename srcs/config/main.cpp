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
        std::cout << "PRINGTING THE STRUCT : \n"; 

        std::cout << "iter->host: " << iter->host << std::endl;
        std::cout << "iter->port: " ;
        for ( std::vector<int>::iterator iter_port = iter->port.begin(); iter_port != iter->port.end(); iter_port++)
        {
            std::cout << *iter_port << " ";
        }
        std::cout << std::endl;
        std::cout << "iter->root: " << iter->root << std::endl;
        std::cout << "iter->client_max_body_size: " << iter->client_max_body_size << std::endl;
        
        std::cout << "error pages     : "  << std::endl; 
        std::map<int, std::string>::iterator    itr = iter->error_pages.begin();
        for (;itr != iter->error_pages.end(); itr++)
        {
            std::cout << "error number:" << itr->first << " -> pages: " << itr->second << std::endl;

        }

        std::cout << "cgi_extension: " << std::endl;
        for ( std::map<std::string, std::string>::iterator cgi_exten_iter = iter->cgi_extension.begin()
                ; cgi_exten_iter != iter->cgi_extension.end(); cgi_exten_iter++)
        {
            std::cout << cgi_exten_iter->first << " " << cgi_exten_iter->second << std::endl;

        }

        std::cout << "location: " << std::endl;
        for (std::vector<LocationConfig>::iterator iter_loc  = iter->locations.begin(); iter_loc != iter->locations.end(); ++iter_loc)
        {
            std::cout << "path: " << iter_loc->path << std::endl;
            std::cout << "root: " << iter_loc->root << std::endl;
            std::cout << "redirection: " << iter_loc-> redirection_status << " " << iter_loc->redirection_url << std::endl;
            std::cout << "index: ";
            for (std::vector<std::string>::iterator iter_index = iter_loc->index.begin(); iter_index != iter_loc->index.end(); iter_index++ )
            {
                std::cout << *iter_index << " " ;
            }
            std::cout << std::endl;

            std::cout << "autoindex: " << iter_loc->autoindex << std::endl;
            std::cout << "upload_store: " << iter_loc->upload_store << std::endl;
            // std::cout << "cgi_pass: " << iter_loc->cgi_pass << std::endl;
            std::cout << "allowed_methods: ";

            for (std::vector<std::string>::iterator iter_meths = iter_loc->allowed_methods.begin(); iter_meths != iter_loc->allowed_methods.end(); ++iter_meths)
            {
                std::cout << *iter_meths << "  "; 
            }

            std::cout << std::endl;
        }
    }
    
}