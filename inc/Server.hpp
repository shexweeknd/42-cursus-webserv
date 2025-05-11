#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
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

class Server
{
    private:
        int             configServerSocket();
        void            configServerAddr();
        int             bindThemUp();
        int             configEpoll();
        void            listenOnClients(epoll_event &events);
        void            handleClient(int clientFd);
        void            handleRequest(int clientFd, std::string &request);
        std::string     waitForRequest(int clientFd);
        void            setNonBlocking(int fd);
        void            watchForEvents();
        void            sendFile(int clientSocket, const std::string &filePath);

    public:
        Server();
        ~Server();
        
        int         serverFd;
        int         epollFd;
        sockaddr_in serverAddr;
        epoll_event ev;
        epoll_event events[MAX_EVENTS];
        int         confListening();
};
