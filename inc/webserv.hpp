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