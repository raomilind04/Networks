#ifndef UTILS_SOCKETUTILS_H
#define UTILS_SOCKETUTILS_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

struct sockaddr_in* createAddress(char* ip, int port); 

int createTCPSocket(); 

#endif  
