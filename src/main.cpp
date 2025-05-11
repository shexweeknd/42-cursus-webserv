#include "webserv.hpp"

int main(void)
{
    try
    {
        Server server;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
    return (0);
}