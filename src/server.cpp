
#include "webserv.hpp"

int main(void)
{    
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        return (1);
    }
    std::cout << "Socket created successfully." << std::endl;

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding socket: " << strerror(errno) << std::endl;
        close(serverSocket);
        return (1);
    }
    std::cout << "Socket bound successfully." << std::endl;
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening on socket: " << strerror(errno) << std::endl;
        close(serverSocket);
        return (1);
    }
    std::cout << "Listening on port 8080." << std::endl;
    
    //accept connections
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
        close(serverSocket);
        return (1);
    }
    std::cout << "Connection accepted." << std::endl;
    //receive data
    char buffer[1024];
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived == -1) {
        std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
        close(clientSocket);
        close(serverSocket);
        return (1);
    }
    buffer[bytesReceived] = '\0'; // Null-terminate the received data
    std::cout << "Received data: " << buffer << std::endl;
    //send data
    const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, world!";
    ssize_t bytesSent = send(clientSocket, response, strlen(response), 0);
    if (bytesSent == -1) {
        std::cerr << "Error sending data: " << strerror(errno) << std::endl;
        close(clientSocket);
        close(serverSocket);
        return (1);
    }
    std::cout << "Response sent." << std::endl;
    //close sockets
    close(clientSocket);
    close(serverSocket);
    std::cout << "Sockets closed." << std::endl;
    std::cout << "Server shutting down." << std::endl;
    return (0);
}