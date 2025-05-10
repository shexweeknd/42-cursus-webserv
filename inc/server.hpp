#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>  // Pour getaddrinfo() et gai_strerror()
#include <unistd.h> // Pour close()
#include <cstring>  // Pour memset()
#include <errno.h>  // Pour errno

int configServerSocket(void);
int bindThemUp(int serverSocket, sockaddr_in &serverAddr);
int listenOnSocket(int serverSocket);
void configServerAddr(sockaddr_in &serverAddr);
