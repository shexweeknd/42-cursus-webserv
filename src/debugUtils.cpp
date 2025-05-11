#include "debugUtils.hpp"

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
// close function

void sendHTML(int clientSocket)
{
    const char *htmlResponse = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/html\r\n"
                               "Content-Length: 13\r\n"
                               "\r\n"
                               "<h1>Hello</h1>\n";
    
    send(clientSocket, htmlResponse, strlen(htmlResponse), 0);
    close(clientSocket);
}