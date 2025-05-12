#include "webserv.hpp"

int main(void)
{
    try
    {
        std::vector<int> ports;
        ports.push_back(8080);
        ports.push_back(8081);
        
        Server server(ports);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
    return (0);
}