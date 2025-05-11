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

void work(Server &server)
{
    struct epoll_event ev, events[MAX_EVENTS];
    
    std::cout << "\e[32mWorker process (" << getpid() << ") is starting...\e[0m" << std::endl;
    
    int epollFd = epoll_create(1);
    if (epollFd == -1) {
        std::cerr << "\e[31mError creating epoll instance: \e[0m" << strerror(errno) << std::endl;
        exit(1);
    }
    ev.events = EPOLLIN;
    ev.data.fd = server.serverFd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, server.serverFd, &ev) == -1) {
        std::cerr << "\e[31mError adding server socket to epoll: \e[0m" << strerror(errno) << std::endl;
        close(epollFd);
        exit(1);
    }
   
    while (true)
    {
        int nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            std::cerr << "\e[31mError in epoll_wait: \e[0m" << strerror(errno) << std::endl;
            close(epollFd);
            exit(1);
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == server.serverFd)
            {
                sockaddr_in clientAddr;
                socklen_t clientAddrLen = sizeof(clientAddr);
                int clientFd = accept(server.serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
                if (clientFd == -1) {
                    std::cerr << "\e[31mError accepting client connection: \e[0m" << strerror(errno) << std::endl;
                    continue;
                }
                std::cout << "\e[32mAccepted new client connection: \e[0m" << clientFd << std::endl;
                fcntl(clientFd, F_SETFL, O_NONBLOCK);
                // ev.events = EPOLLIN | EPOLLET; // Edge-triggered need to get all data with one read
                ev.events = EPOLLIN; // Level-triggered
                ev.data.fd = clientFd;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
                    std::cerr << "\e[31mError adding client socket to epoll: \e[0m" << strerror(errno) << std::endl;
                    close(clientFd);
                    continue;
                }
                std::cout << "\e[32mClient socket added to epoll: \e[0m" << clientFd << std::endl;
            } else {
                // Handle client request
                char buffer[BUFFER_SIZE];
                ssize_t bytesReceived = recv(events[n].data.fd, buffer, sizeof(buffer) - 1, 0);
                if (bytesReceived == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        continue;
                    } else {
                        close(events[n].data.fd);
                    }
                } else if (bytesReceived == 0) {
                    // Client disconnected
                    std::cout << "\e[32mClient disconnected: \e[0m" << events[n].data.fd << std::endl;
                    close(events[n].data.fd);
                    continue;
                }
                buffer[bytesReceived] = '\0';
                std::cout << "\e[32mReceived data from client: \e[0m" << buffer << std::endl;

                // Send HTML response
                sendHTML(events[n].data.fd);
                std::cout << "\e[32mHTML response sent to client: \e[0m" << events[n].data.fd << std::endl;
                if (!isKeepAlive(buffer)) {
                    std::cout << "\e[31mKeep-alive: false detected closing client fd\e[0m" << std::endl;
                    close(events[n].data.fd);
                    std::cout << "\e[32mClosed client socket: \e[0m" << events[n].data.fd << std::endl;
                }
            }
        }
    }
}