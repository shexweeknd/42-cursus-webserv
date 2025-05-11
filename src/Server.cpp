#include "Server.hpp"

Server::Server()
{
    serverFd = configServerSocket();
    if (serverFd == -1)
        throw std::runtime_error("Failed to create server socket");

    configServerAddr();

    if (bindThemUp() == -1)
        throw std::runtime_error("Failed to bind server socket");
    if (listenOnSocket() == -1)
        throw std::runtime_error("Failed to listen on server socket");
    std::cout << std::endl << "\e[32mServer socket created and configured successfully.\e[0m" << std::endl << std::endl;
}

Server::~Server()
{
    if (close(serverFd) == -1) {
        std::cerr << "\e[31mError closing server socket: \e[0m" << strerror(errno) << std::endl;
    } else {
        std::cout << "\e[32mServer socket closed successfully.\e[0m" << std::endl;
    }
}

// Create && config the server socket
int Server::configServerSocket(void)
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
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);
    std::cout << "\e[32mSocket created successfully.\e[0m" << std::endl;
    return (serverSocket);
}

// Set up the server address structure
void Server::configServerAddr()
{
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
}

// Bind the socket to the address and port
int Server::bindThemUp()
{
    if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "\e[31mError binding socket: " << strerror(errno) << std::endl;
        close(serverFd);
        return (1);
    }
    std::cout << "\e[32mSocket bound successfully.\e[0m" << std::endl;
    return (0);
}

// Set the socket to listen for incoming connections
int Server::listenOnSocket()
{
    fcntl(serverFd, F_SETFL, O_NONBLOCK);
    if (listen(serverFd, SOMAXCONN) == -1) {
        std::cerr << "\e[31mError listening on socket: \e[0m" << strerror(errno) << std::endl;
        close(serverFd);
        return (1);
    }
    std::cout << "\e[32mListening on port:\e[0m 8080" << std::endl;
    return (0);
}