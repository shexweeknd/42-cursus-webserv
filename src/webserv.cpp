#include "webserv.hpp"

int isKeepAlive(const char *buffer)
{
    std::string request(buffer);
    if (request.find("Connection: keep-alive") != std::string::npos) {
        return (1);
    }
    if (request.find("alive") != std::string::npos) {
        return (1);
    }
    return (0);
}
