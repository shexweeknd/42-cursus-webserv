#pragma once

#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <sys/socket.h> 
#include <cstring>
#include <unistd.h>

void printClientInfo(const sockaddr_in &clientAddr);
void sendHTML(int clientSocket);