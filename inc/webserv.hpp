#pragma once

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>  // Pour getaddrinfo() et gai_strerror()

#include <unistd.h> // Pour close()
#include <cstring>  // Pour memset()
#include <cstdlib> // Pour exit()
#include <errno.h>  // Pour errno
#include <arpa/inet.h> // Pour inet_ntop()
#include <sys/epoll.h> // Pour epoll
#include <fcntl.h> // Pour fcntl
#include <sys/wait.h> // Pour waitpid
#include <signal.h> // Pour signal
#include <sys/stat.h> // Pour stat
#include <sys/types.h> // Pour stat
#include <sys/socket.h> // Pour socket

// other headers
#include "debugUtils.hpp"
#include "Server.hpp"

void work(Server &server);