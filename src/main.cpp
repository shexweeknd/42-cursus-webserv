#include "webserv.hpp"

#define GREEN "\033[32m"

void printClientInfo(const sockaddr_in &clientAddr)
{
    // Print client address
    std::cout << std::endl;
    std::cout << "\e[32mConnection from client : \e[0m" << std::endl;
    
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
    std::cout << "Family: " << clientAddr.sin_family << std::endl;
    std::cout << "Client IP: " << clientIP << std::endl;
    std::cout << "Client port: " << ntohs(clientAddr.sin_port) << std::endl;
    std::cout << "Client address: " << clientAddr.sin_addr.s_addr << std::endl;
    
    std::cout << std::endl;
}

int main(void)
{
    sockaddr_in serverAddr;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "\e[31mError creating socket: \e[0m" << strerror(errno) << std::endl;
        return (1);
    }
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "\e[31mError setting socket options: \e[0m" << strerror(errno) << std::endl;
        close(serverSocket);
        return (1);
    }
    std::cout << "\e[32mSocket created successfully.\e[0m" << std::endl;

    // Set up the server address structure
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    // Bind the socket to the address and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "\e[31mError binding socket: " << strerror(errno) << std::endl;
        close(serverSocket);
        return (1);
    }
    std::cout << "\e[32mSocket bound successfully.\e[0m" << std::endl;

    // Set the socket to listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "\e[31mError listening on socket: \e[0m" << strerror(errno) << std::endl;
        close(serverSocket);
        return (1);
    }
    std::cout << "\e[32mListening on port 8080.\e[0m" << std::endl;

    //accept connections
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        std::cerr << "\e[31mError accepting connection: \e[0m" << strerror(errno) << std::endl;
        close(serverSocket);
        return (1);
    }

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