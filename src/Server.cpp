#include "Server.hpp"

Server::Server()
{
    serverFd = configServerSocket();
    if (serverFd == -1)
        throw std::runtime_error("Failed to create server socket");

    configServerAddr();

    if (bindThemUp() == -1)
        throw std::runtime_error("Failed to bind server socket");
    if (confListening() == -1)
        throw std::runtime_error("Failed to listen on server socket");
    if (configEpoll() == -1)
        throw std::runtime_error("Failed to configure epoll");
    std::cout << std::endl << "\e[32mServer socket created and configured successfully.\e[0m" << std::endl << std::endl;
    std::cout << "\e[32mServer is running...\e[0m" << std::endl;
    watchForEvents();
    std::cout << "\e[32mServer is shutting down...\e[0m" << std::endl;
}

Server::~Server()
{
    if (close(serverFd) == -1) {
        std::cerr << "\e[31mError closing server socket: \e[0m" << strerror(errno) << std::endl;
    } else {
        std::cout << "\e[32mServer socket closed successfully.\e[0m" << std::endl;
    }
    if (close(epollFd) == -1) {
        std::cerr << "\e[31mError closing epoll instance: \e[0m" << strerror(errno) << std::endl;
    } else {
        std::cout << "\e[32mEpoll instance closed successfully.\e[0m" << std::endl;
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
// TODO make it listen on multiple ports
// TODO make it listen on multiple addresses
int Server::confListening()
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

void Server::setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "\e[31mError getting flags: \e[0m" << strerror(errno) << std::endl;
        return;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "\e[31mError setting non-blocking mode: \e[0m" << strerror(errno) << std::endl;
    }
}

// Listen for incoming client connections
void Server::listenOnClients(epoll_event &events)
{
    int clientFd = accept(serverFd, NULL, NULL);
    if (clientFd == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // No more connections to accept
        } else {
            std::cerr << "\e[31mError accepting connection: \e[0m" << strerror(errno) << std::endl;
        }
    }
    else if (clientFd > MAX_CLIENTS) {
        std::cerr << "\e[31mToo many clients connected: \e[0m" << clientFd << std::endl;
        close(clientFd);
        return;
    }
    std::cout << "\e[32mClient connected: \e[0m" << clientFd << std::endl;
    events.events = EPOLLIN | EPOLLET;
    events.data.fd = clientFd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &events) == -1) {
        std::cerr << "\e[31mError adding client socket to epoll: \e[0m" << strerror(errno) << std::endl;
        close(clientFd);
        return;
    }
    std::cout << "\e[32mClient socket added to epoll successfully.\e[0m" << std::endl;
    setNonBlocking(clientFd);
}

void Server::handleClient(int clientFd)
{
    std::cout << "\e[32mNew client connected: \e[0m" << clientFd << std::endl;
    std::string request = waitForRequest(clientFd);
    if (request.empty()) {
        std::cerr << "\e[31mError receiving request: \e[0m" << strerror(errno) << std::endl;
        close(clientFd);
        return;
    }
    handleRequest(clientFd, request);
    close(clientFd);
}

// TODO parse the request and send a response
void Server::handleRequest(int clientFd, std::string &request)
{
    (void)request;
    std::cout << "\e[32mHandling request from client: \e[0m" << clientFd << std::endl;
    sendFile(clientFd, "html/index.html");
}

void Server::watchForEvents()
{
    while (true) {
        int eventCount = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (eventCount == -1) {
            std::cerr << "\e[31mError waiting for events: \e[0m" << strerror(errno) << std::endl;
            continue;
        }
        for (int i = 0; i < eventCount; i++) {
            if (events[i].data.fd == serverFd) {
                listenOnClients(events[i]);
            } else if (events[i].events & EPOLLERR) {
                std::cerr << "\e[31mError on client socket: \e[0m" << events[i].data.fd << std::endl;
                close(events[i].data.fd);
            } else if (events[i].events & EPOLLHUP) {
                std::cerr << "\e[31mClient disconnected: \e[0m" << events[i].data.fd << std::endl;
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
        std::cerr << "\e[31mError receiving data: \e[0m" << strerror(errno) << std::endl;
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
        std::cerr << "\e[31mError creating epoll instance: \e[0m" << strerror(errno) << std::endl;
        return (-1);
    }
    ev.events = EPOLLIN;
    ev.data.fd = serverFd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &ev) == -1)
    {
        std::cerr << "\e[31mError adding server socket to epoll: \e[0m" << strerror(errno) << std::endl;
        close(epollFd);
        return (-1);
    }
    std::cout << "\e[32mEpoll instance created and server socket added successfully.\e[0m" << std::endl;
    return (epollFd);
}

// Send HTML file directly to the client
void Server::sendFile(int clientSocket, const std::string &filePath)
{
    int file = open(filePath.c_str(), O_RDONLY);
    if (file == -1) {
        std::cerr << "\e[31mError opening file: \e[0m" << strerror(errno) << std::endl;
        return;
    }
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) == -1) {
        std::cerr << "\e[31mError getting file stats: \e[0m" << strerror(errno) << std::endl;
        close(file);
        return;
    }
    char *fileBuffer = new char[fileStat.st_size];
    ssize_t bytesRead = read(file, fileBuffer, fileStat.st_size);
    if (bytesRead == -1) {
        std::cerr << "\e[31mError reading file: \e[0m" << strerror(errno) << std::endl;
        delete[] fileBuffer;
        close(file);
        return;
    }
    std::ostringstream ss;
    ss << fileStat.st_size;
    std::string response = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n"
                            "Content-Length: " + ss.str() + "\r\n"
                            "\r\n";
    response += fileBuffer;
    ssize_t bytesSent = send(clientSocket, response.c_str(), response.size(), 0);
    if (bytesSent == -1) {
        std::cerr << "\e[31mError sending file: \e[0m" << strerror(errno) << std::endl;
    }
    delete[] fileBuffer;
    close(file);
    std::cout << "\e[32mFile sent successfully.\e[0m" << std::endl;
}