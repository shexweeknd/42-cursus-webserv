#include "Server.hpp"

// canonical form
Server::Server(const Server &src)
{
    *this = src;
}

Server &Server::operator=(const Server &src)
{
    if (this != &src) {
        for (std::vector<int>::iterator it = serverFds.begin(); it != serverFds.end(); ++it) {
            if (close(*it) == -1) {
                std::cerr << "\e[31mError closing server socket : \e[0m" << *it << std::endl;
            } else {
                std::cout << "\e[32mServer socket closed successfully.\e[0m" << std::endl;
            }
        }
        if (close(epollFd) == -1) {
            std::cerr << "\e[31mError closing epoll instance : \e[0m" << epollFd << std::endl;
        } else {
            std::cout << "\e[32mEpoll instance closed successfully.\e[0m" << std::endl;
        }
        *this = Server(src.ports);
    }
    return (*this);
}

Server::Server()
{}

Server::~Server()
{
    for (std::vector<int>::iterator it = serverFds.begin(); it != serverFds.end(); ++it) {
        if (close(*it) == -1) {
            std::cerr << "\e[31mError closing server socket : \e[0m" << *it << std::endl;
        } else {
            std::cout << "\e[32mServer socket closed successfully.\e[0m" << std::endl;
        }
    }
    if (close(epollFd) == -1) {
        std::cerr << "\e[31mError closing epoll instance: \e[0m" << epollFd << std::endl;
    } else {
        std::cout << "\e[32mEpoll instance closed successfully.\e[0m" << std::endl;
    }
    std::cout << "\e[32mServer socket closed successfully.\e[0m" << std::endl;
}

// Constructor that takes a vector of ports
Server::Server(std::vector<int> ports)
{
    this->ports = ports;
    memset(events, 0, sizeof(events));
    int epollFd = configEpoll();
    if (epollFd == -1)
        throw std::runtime_error("Failed to configure epoll");
    for (std::vector<int>::iterator it = ports.begin(); it != ports.end(); ++it) {
        int serverFd;
        serverFd = configServerSocket();
        if (serverFd == -1)
            throw std::runtime_error("Failed to create server socket");
        configServerAddr(*it);
        if (bindThemUp(serverFd) == -1)
            throw std::runtime_error("Failed to bind server socket");
        if (confListening(serverFd) == -1)
            throw std::runtime_error("Failed to listen on server socket");
        if (addServerFdToEpoll(epollFd, serverFd) == -1)
            throw std::runtime_error("Failed to add server socket to epoll");
        serverFds.push_back(serverFd);
        std::cout << std::endl;
    }
    std::cout << "\e[32mServer is running waiting for events ...\e[0m" << std::endl;
    watchForEvents();
    std::cout << "\e[32mServer is shutting down...\e[0m" << std::endl;
}

// Create && config the server socket
int Server::configServerSocket(void)
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "\e[31mError creating socket : \e[0m" << "socket() returned -1" << std::endl;
        return (-1);
    }
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "\e[31mError setting socket options for serverSocket : \e[0m" << "setsockopt() returned -1" << std::endl;
        close(serverSocket);
        return (-1);
    }
    if (setNonBlocking(serverSocket) == -1) {
        std::cerr << "\e[31mError setting non-blocking mode for serverSocket : \e[0m" << "fcntl() returned -1" << std::endl;
        close(serverSocket);
        return (-1);
    }
    std::cout << "\e[32mSocket created successfully.\e[0m" << std::endl;
    return (serverSocket);
}

// Set up the server address structure
void Server::configServerAddr(unsigned short port)
{
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    std::cout << "\e[32mServer address configured to listen on port :\e[0m" << port << std::endl;
}

// Bind the socket to the address and port
int Server::bindThemUp(int serverFd)
{
    if (serverFd == -1) {
        std::cerr << "\e[31mError: serverFd is -1\e[0m" << std::endl;
        return (1);
    }
    if (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "\e[31mError binding socket : " << serverFd << std::endl;
        close(serverFd);
        return (1);
    }
    std::cout << "\e[32mSocket bound successfully.\e[0m" << std::endl;
    return (0);
}

// Set the socket to listen for incoming responseections
int Server::confListening(int serverFd)
{
    if (serverFd == -1) {
        std::cerr << "\e[31mError: serverFd is -1\e[0m" << std::endl;
        return (1);
    }
    // Set the socket to non-blocking mode
    if (setNonBlocking(serverFd) == -1) {
        std::cerr << "\e[31mError setting non-blocking mode for serverFd : \e[0m" << serverFd << std::endl;
        close(serverFd);
        return (1);
    }
    // Listen for incoming responseections
    if (listen(serverFd, SOMAXCONN) == -1) {
        std::cerr << "\e[31mError listening on socket : \e[0m" << serverFd << std::endl;
        close(serverFd);
        return (1);
    }
    std::cout << "\e[32mServer :\e[0m " << serverFd << "\e[32m is ready to listen.\e[0m" << std::endl;
    return (0);
}

// Add the server socket to the epoll instance
int Server::addServerFdToEpoll(int epollFd, int serverFd)
{
    if (epollFd == -1 || serverFd == -1) {
        std::cerr << "\e[31mError: epollFd or serverFd is -1\e[0m" << std::endl;
        return (-1);
    }
    ev.events = EPOLLIN;
    ev.data.fd = serverFd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &ev) == -1)
    {
        std::cerr << "\e[31mError adding serverFd \e[0m" << serverFd << "\e[31m to epoll : \e[0m" << epollFd  << std::endl;
        close(epollFd);
        return (-1);
    }
    std::cout << "\e[32mServer socket :\e[0m " << serverFd << "\e[32m added to epoll successfully.\e[0m" << std::endl;
    return (0);
}

int Server::setNonBlocking(int fd)
{
    return (fcntl(fd, F_SETFL, O_NONBLOCK));
}

// Listen for incoming client responseections
void Server::listenOnClients(epoll_event &events, int serverFd)
{
    int clientFd = accept(serverFd, NULL, NULL);
    if (clientFd == -1) {
        std::cout << "\e[31mError accepting client responseection: \e[0m" << "accept() returned -1" << std::endl;
    }
    else if (clientFd > MAX_CLIENTS) {
        std::cerr << "\e[31mToo many clients responseected: \e[0m" << clientFd << std::endl;
        close(clientFd);
        return;
    }
    std::cout << "\e[32mClient responseected: \e[0m" << clientFd << std::endl;
    events.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    events.data.fd = clientFd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &events) == -1) {
        std::cerr << "\e[31mError adding client socket to epoll: \e[0m" << "epoll_ctl() returned -1" << std::endl;
        close(clientFd);
        return;
    }
    std::cout << "\e[32mClient socket added to epoll successfully.\e[0m" << std::endl;
    if (setNonBlocking(clientFd) == -1) {
        std::cerr << "\e[31mError setting non-blocking mode for client socket: \e[0m" << clientFd << std::endl;
        close(clientFd);
        return;
    }
}

void Server::handleClient(int clientFd)
{
    std::cout << "\e[32mNew client responseected: \e[0m" << clientFd << std::endl;
    std::string request = waitForRequest(clientFd);
    if (request.empty()) {
        std::cerr << "\e[31mGot an empty request from client: \e[0m" << clientFd << std::endl;
        return;
    }
    handleRequest(clientFd, request);
    bool isKeepAlive = (request.find("Connection: keep-alive") != std::string::npos);
    if (!isKeepAlive) {
        // remove client from epoll
        if (epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, NULL) == -1) {
            std::cerr << "\e[31mError removing client socket from epoll: \e[0m" << "epoll_ctl() returned -1" << std::endl;
        }
        std::cout << "\e[32mClient socket removed from epoll successfully.\e[0m" << std::endl;
        // close the client socket
        shutdown(clientFd, SHUT_WR);
        if (close(clientFd) == -1) {
            std::cerr << "\e[31mError closing client socket: \e[0m" << clientFd << std::endl;
        } else {
            std::cout << "\e[32mClient socket closed successfully.\e[0m" << std::endl;
        }
        std::cout << "\e[32mClient disresponseected: \e[0m" << clientFd << std::endl;
    }
    else {
        std::cout << "\e[32mClient socket kept alive.\e[0m" << std::endl;
    }
}

t_resp Server::configResponse(std::map<std::string, std::string> reqHeaders)
{
    t_resp  response;
    bool    alive = false;

    //TODO change
    std::string rootPath = "./html/";

    // Header keys
    response.contentTypeHeader = "Content-Type";
    response.contentLengthHeader = "Content-Length";
    response.connectionHeader = "Connection";
    response.lastModifiedHeader = "Last-Modified";
    response.acceptRangesHeader = "Accept-Ranges";
    response.contentDispositionHeader = "Content-Disposition";
    response.contentTransferEncodingHeader = "Content-Transfer-Encoding";
    response.serverHeader = "Server";
    response.dateHeader = "Date";

    if (reqHeaders.find("Host") == reqHeaders.end()) {
        std::cerr << "\e[31mError: Host header not found in request.\e[0m" << std::endl;
        response.statusCode = 400;
        response.statusMessage = "Bad Request";
        response.path = "./html/400.html";
        return (response);
    }
    // verifier le path si la syntaxe est correcte error 400
    if (reqHeaders["path"][0] != '/'
        || reqHeaders["path"].find("..") != std::string::npos
        || reqHeaders["path"].find("~") != std::string::npos
        || reqHeaders["path"].empty()) {
        std::cerr << "\e[31mError: path header not found in request.\e[0m" << std::endl;
        response.statusCode = 400;
        response.statusMessage = "Bad Request";
        response.path = "./html/400.html";
        return (response);
    }
    
    response.path = reqHeaders["path"];
    response.path.erase(0, 1);

    // si path == '/' on renvoie index.html
    if (reqHeaders["path"].compare("/") == 0) {
        response.path = rootPath + "/index.html";
        response.statusCode = 200;
        response.statusMessage = "OK";
        return (response);
    }
    
    //  verifier si le path existe error 404
    if (access(response.path.c_str(), F_OK) == -1) {
        std::cerr << "\e[31mError: path header not found in request.\e[0m" << std::endl;
        response.statusCode = 404;
        response.statusMessage = "Not Found";
        response.path = "./html/404.html";
        return (response);
    }

    // verifier 408 error
    if (reqHeaders.find("Content-Length") == reqHeaders.end()) {
        std::cerr << "\e[31mError: Content-Length header not found in request.\e[0m" << std::endl;
        response.statusCode = 408;
        response.statusMessage = "Request Timeout";
        response.path = "./html/408.html";
        return (response);
    }

    if (reqHeaders.find("Connection") != reqHeaders.end()) {
        if (reqHeaders["Connection"] == "keep-alive") {
            alive = true;
        } else {
            alive = false;
        }
    }

    response.statusCode = 200;
    response.statusMessage = "OK";

    // header values
    response.contentTypeHeaderValue = "text/html";
    response.contentLengthHeaderValue = "0";
    response.connectionHeaderValue = (alive ? "keep-alive" : "close");
    response.lastModifiedHeaderValue = std::string(__DATE__);
    response.acceptRangesHeaderValue = "bytes";
    response.contentDispositionHeaderValue = "inline";
    response.contentTransferEncodingHeaderValue = "binary";
    response.serverHeaderValue = "Webserv";
    response.dateHeaderValue = std::string(__DATE__);

    return (response);
}

// TODO parse the request and send a response
void Server::handleRequest(int clientFd, std::string &request)
{
    if (request.empty()) {
        std::cerr << "\e[31mGot an empty request from client: \e[0m" << clientFd << std::endl;
        return;
    }
    std::map<std::string, std::string> reqHeaders = parseHttpRequest(request);
    t_resp resp;
    resp = configResponse(reqHeaders);
    std::cout << "\e[32mHandling request from client: \e[0m" << clientFd << std::endl;
    sendFile(clientFd, resp);
}

void Server::watchForEvents()
{
    while (true) {
        memset(events, 0, sizeof(events));
        int eventCount = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (eventCount == -1) {
            std::cerr << "\e[31mError waiting for events: \e[0m" << "epoll_wait() returned -1" << std::endl;
            continue;
        }
        for (int i = 0; i < eventCount; i++) {
            std::vector<int>::iterator it = std::find(serverFds.begin(), serverFds.end(), events[i].data.fd);
            if (it != serverFds.end()) {
                listenOnClients(events[i], *it);
            } else if (events[i].events & EPOLLERR) {
                std::cerr << "\e[31mError on client socket: \e[0m" << events[i].data.fd << std::endl;
                close(events[i].data.fd);
            } else if (events[i].events & EPOLLHUP) {
                std::cerr << "\e[31mClient disresponseected: \e[0m" << events[i].data.fd << std::endl;
                close(events[i].data.fd);
            } else if (events[i].events & EPOLLIN) {
                std::cout << "\e[32mData available to read from client: \e[0m" << events[i].data.fd << std::endl;
                handleClient(events[i].data.fd);
            // } else if (events[i].events & EPOLLOUT) {
            //     std::cout << "\e[32mData available to write to client: \e[0m" << events[i].data.fd << std::endl;
            //     handleClient(events[i].data.fd);
            }
        }
    }
}

std::string Server::waitForRequest(int clientFd)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    std::cout << "\e[32mWaiting for request...\e[0m" << std::endl;
    ssize_t bytesRead = recv(clientFd, buffer, BUFFER_SIZE - 1, 0);
    if (bytesRead == -1) {
        std::cerr << "\e[31mError receiving data from clientFd : \e[0m" << clientFd << std::endl;
    }
    buffer[bytesRead] = '\0';
    std::cout << "\e[32mReceived request:\e[0m " << buffer << std::endl;
    std::string request(buffer);
    return (request);
}

// Create epoll instance
int Server::configEpoll()
{
    epollFd = epoll_create(1);
    if (epollFd == -1)
    {
        std::cerr << "\e[31mError creating epoll instance: \e[0m" << "epoll_create() returned -1" << std::endl;
        return (-1);
    }
    return (epollFd);
}

// Send HTML file directly to the client
void Server::sendFile(int clientSocket, t_resp resp)
{
    int file = open(resp.path.c_str(), O_RDONLY);
    if (file == -1) {
        std::cerr << "\e[31mError opening file : \e[0m" << resp.path << std::endl;
        return;
    }
    struct stat fileStat;
    if (stat(resp.path.c_str(), &fileStat) == -1) {
        std::cerr << "\e[31mError getting file stats for : \e[0m" << resp.path << std::endl;
        close(file);
        return;
    }
    char *fileBuffer = new char[fileStat.st_size];
    ssize_t bytesRead = read(file, fileBuffer, fileStat.st_size);
    if (bytesRead == -1) {
        std::cerr << "\e[31mError reading file : \e[0m" << resp.path << std::endl;
        delete[] fileBuffer;
        close(file);
        return;
    }
    std::ostringstream ssSize;
    std::ostringstream ssStatus;
    ssStatus << resp.statusCode;
    ssSize << fileStat.st_size;
    std::string response = "HTTP/1.1 " + ssStatus.str() +
                            " " + resp.statusMessage + "\r\n"
                            "Server: Webserv\r\n"
                            "Date: " + std::string(__DATE__) + "\r\n"
                            "Last-Modified: " + std::string(__DATE__) + "\r\n"
                            "Accept-Ranges: bytes\r\n"
                            "Content-Disposition: inline; filename=\"" + resp.path + "\"\r\n"
                            "Content-Transfer-Encoding: binary\r\n"
                            "Connection: " + resp.connectionHeaderValue + "\r\n"
                            "Content-Type: text/html\r\n"
                            "Content-Length: " + ssSize.str() + "\r\n" + "\r\n";
    response += fileBuffer;
    ssize_t bytesSent = send(clientSocket, response.c_str(), response.size(), 0);
    if (bytesSent == -1) {
        std::cerr << "\e[31mError sending file : \e[0m" << resp.path << std::endl;
    }
    delete[] fileBuffer;
    close(file);
    std::cout << "\e[32mFile sent successfully.\e[0m" << std::endl;
}