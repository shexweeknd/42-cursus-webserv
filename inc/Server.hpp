#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include <algorithm>
#include <netdb.h>  // Pour getaddrinfo() et gai_strerror()
#include <unistd.h> // Pour close()
#include <cstring>  // Pour memset()
#include <errno.h>  // Pour errno
#include <sys/fcntl.h> // Pour fcntl
#include <sys/epoll.h> // Pour epoll;

#include "debugUtils.hpp"
#include "webserv.hpp"

#define PORT 8080
#define MAX_CLIENTS 100
#define MAX_EVENTS 10
#define BUFFER_SIZE 2048
#define WORKERS 1

typedef struct  s_conn {
    int         status;
    std::string statusMessage;
}               t_conn;

class Server
{
    private:
        Server();
        Server(const Server &src);
        Server &operator=(const Server &src);

        int             configServerSocket();
        void            configServerAddr(unsigned short port = PORT);
        int             bindThemUp(int serverFd);
        int             confListening(int serverFd);
        int             configEpoll();
        int             addServerFdToEpoll(int epollFd, int serverFd);
        void            listenOnClients(epoll_event &events, int serverFd);
        void            handleClient(int clientFd);
        void            handleRequest(int clientFd, std::string &request);
        std::string     waitForRequest(int clientFd);
        void            setNonBlocking(int fd);
        void            watchForEvents();
        void            sendFile(int clientSocket, const std::string &filePath, t_conn conn, int alive);
        
        std::vector<int>    serverFds;
        std::vector<int>    ports;
        int                 epollFd;
        sockaddr_in         serverAddr;
        epoll_event         ev;
        epoll_event         events[MAX_EVENTS];

    public:
        ~Server();
        Server(std::vector<int> ports);

};
