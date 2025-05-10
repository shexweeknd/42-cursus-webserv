#include "webserv.hpp"

int main(void)
{
    sockaddr_in serverAddr;
    int serverSocket = configServerSocket();
    if (serverSocket == -1) return (1);

    configServerAddr(serverAddr);

    if(bindThemUp(serverSocket, serverAddr) == -1) return (1);

    if (listenOnSocket(serverSocket) == -1) return (1);

    sockaddr_in clientAddr;
    int clientSocket = configClientSocket(serverSocket, clientAddr);
    if (clientSocket == -1) return (1);

    printClientInfo(clientAddr);

    //receive data
    char buffer[1024];
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived == -1) {
        std::cerr << "\e[31mError receiving data: \e[0m" << strerror(errno) << std::endl;
        close(clientSocket);
        close(serverSocket);
        return (1);
    }
    buffer[bytesReceived] = '\0'; // Null-terminate the received data
    std::cout << "\e[32mReceived data: \e[0m" << std::endl << buffer << std::endl;

    // Close connexion
    if (close(clientSocket) == -1) {
        std::cerr << "\e[31mError closing client socket: \e[0m" << strerror(errno) << std::endl;
        close(serverSocket);
        return (1);
    }
    if (close(serverSocket) == -1) {
        std::cerr << "\e[31mError closing server socket: \e[0m" << strerror(errno) << std::endl;
        return (1);
    }
    std::cout << "Client socket closed successfully." << std::endl;
    std::cout << "Server socket closed successfully." << std::endl;
    return (0);
}