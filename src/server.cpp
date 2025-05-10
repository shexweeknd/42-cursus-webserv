#include "server.hpp"

// Create && config the server socket
int configServerSocket(void)
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "\e[31mError creating socket: \e[0m" << strerror(errno) << std::endl;
        return (-1);
    }
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "\e[31mError setting socket options: \e[0m" << strerror(errno) << std::endl;
        close(serverSocket);
        return (-1);
    }
    std::cout << "\e[32mSocket created successfully.\e[0m" << std::endl;
    return (serverSocket);
}

// Set up the server address structure
void configServerAddr(sockaddr_in &serverAddr)
{
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);
}

// Bind the socket to the address and port
int bindThemUp(int serverSocket, sockaddr_in &serverAddr)
{
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "\e[31mError binding socket: " << strerror(errno) << std::endl;
        close(serverSocket);
        return (1);
    }
    std::cout << "\e[32mSocket bound successfully.\e[0m" << std::endl;
    return (0);
}

// Set the socket to listen for incoming connections
int listenOnSocket(int serverSocket)
{
    if (listen(serverSocket, SOMAXCONN) == -1) {
        std::cerr << "\e[31mError listening on socket: \e[0m" << strerror(errno) << std::endl;
        close(serverSocket);
        return (1);
    }
    std::cout << "\e[32mListening on port 8080.\e[0m" << std::endl;
    return (0);
}