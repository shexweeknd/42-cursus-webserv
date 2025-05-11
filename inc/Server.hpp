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

#define PORT 8080
#define MAX_CLIENTS 100
#define MAX_EVENTS 10
#define BUFFER_SIZE 2048
#define WORKERS 1

class Server
{
    private:
        int     configServerSocket();
        void    configServerAddr();
        int     bindThemUp();

    public:
        Server();
        ~Server();
        
        int         serverFd;
        sockaddr_in serverAddr;
        int         listenOnSocket();
};
