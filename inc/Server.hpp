#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>  // Pour getaddrinfo() et gai_strerror()
#include <unistd.h> // Pour close()
#include <cstring>  // Pour memset()
#include <errno.h>  // Pour errno
#include <sys/fcntl.h> // Pour fcntl
#include <sys/epoll.h> // Pour epoll;

#include "debugUtils.hpp"
#include "urlParser.hpp"
#include "webserv.hpp"

#define PORT 8080
#define MAX_CLIENTS 100
#define MAX_EVENTS 10
#define BUFFER_SIZE 2048

typedef struct s_resp {
    int         statusCode;
    std::string statusMessage;

    // Headers
    std::string contentTypeHeader;
    std::string contentLengthHeader;
    std::string connectionHeader;
    std::string lastModifiedHeader;
    std::string acceptRangesHeader;
    std::string contentDispositionHeader;
    std::string contentTransferEncodingHeader;
    std::string serverHeader;
    std::string dateHeader;

    // Header values
    std::string contentTypeHeaderValue;
    std::string contentLengthHeaderValue;
    std::string connectionHeaderValue;
    std::string lastModifiedHeaderValue;
    std::string acceptRangesHeaderValue;
    std::string contentDispositionHeaderValue;
    std::string contentTransferEncodingHeaderValue;
    std::string serverHeaderValue;
    std::string dateHeaderValue;

    // Content
    std::string path;
    std::string content;
    std::string method;
    std::string requestUri;
    std::string httpVersion;
}   t_resp;

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
        t_resp          configResponse(std::map<std::string, std::string> reqHeaders);
        std::string     waitForRequest(int clientFd);
        int             setNonBlocking(int fd);
        void            watchForEvents();
        void            sendFile(int clientSocket, t_resp response);
        
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
