#include "client.hpp"

// Accept client connections
int configClientSocket(int serverSocket, sockaddr_in &clientAddr)
{
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        std::cerr << "\e[31mError accepting connection: \e[0m" << strerror(errno) << std::endl;
        close(serverSocket);
        return (-1);
    }
    std::cout << "\e[32mClient connected successfully.\e[0m" << std::endl;
    return (clientSocket);
}