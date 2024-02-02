#include "utils.h"

int createTCPSocket() {
  return socket(AF_INET, SOCK_STREAM, 0); 
}

struct sockaddr_in* createAddress(char* ip, int port) {
  struct sockaddr_in* addr = malloc(sizeof(struct sockaddr_in)); 
  addr->sin_family = AF_INET; 
  addr->sin_port = htons(port); 

  if (strlen(ip) == 0) addr->sin_addr.s_addr = INADDR_ANY;
  else inet_pton(AF_INET, ip, &addr->sin_addr.s_addr); 

  return addr; 
}
