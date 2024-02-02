#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include "utils.h"

struct AcceptedSocket {
  int acceptedSocketFD; 
  struct sockaddr_in addr; 
  int error; 
  bool acceptedSuccessfully; 
}; 

struct AcceptedSocket acceptedSockets[10]; 
int acceptedSocketCount = 0; 

void acceptIncomingConnections(int serverFD);
struct AcceptedSocket* acceptClients(int serverFD); 
void receiveFromClient(struct AcceptedSocket* client);
void getData(int socketFD); 
void sendUpdateToClients(char* buffer, int socketFD); 

int main() {
  int serverFD = createTCPSocket(); 
  struct sockaddr_in* serverAddr = createAddress("", 2000); 

  int result = bind(serverFD, serverAddr, sizeof(*serverAddr)); 
  if (result == 0) printf("Server started at port 2000\n"); 

  int listenResult = listen(serverFD, 10); 
  acceptIncomingConnections(serverFD); 
  shutdown(serverFD, SHUT_RDWR); 

  return 0; 
}

void acceptIncomingConnections(int serverFD) {
  while (true) {
    struct AcceptedSocket* client = acceptClients(serverFD);
    acceptedSockets[acceptedSocketCount++] = *client; 

    receiveFromClient(client);  
  }
}

struct AcceptedSocket* acceptClients(int serverFD) {
  struct sockaddr_in client; 
  int clientAddrSize = sizeof(struct sockaddr_in); 
  int clientFD = accept(serverFD, &client, &clientAddrSize); 

  struct AcceptedSocket* acceptedSocket = malloc(sizeof(struct AcceptedSocket)); 
  acceptedSocket->addr = client; 
  acceptedSocket->acceptedSocketFD = clientFD; 
  acceptedSocket->acceptedSuccessfully = clientFD > 0;

  if (!acceptedSocket->acceptedSuccessfully) acceptedSocket->error = clientFD; 
  
  return acceptedSocket; 
}

void receiveFromClient(struct AcceptedSocket* client) {
  pthread_t id;
  pthread_create(&id, NULL, getData, client->acceptedSocketFD); 
}

void getData(int socketFD) {
  char buffer[1024]; 
  while (true) {
    ssize_t dataSize = recv(socketFD, buffer, 1024, 0); 

    if (dataSize > 0) {
      buffer[dataSize] = 0; 
      printf("%s\n", buffer); 
      sendUpdateToClients(buffer, socketFD); 

    }

    if (dataSize == 0) break; 
  }
  close(socketFD); 
}

void sendUpdateToClients(char* buffer, int socketFD) {
  for (int i = 0; i < acceptedSocketCount; i++) {
    if (acceptedSockets[i].acceptedSocketFD != socketFD) 
      send(acceptedSockets[i].acceptedSocketFD, buffer, strlen(buffer), 0); 
  }
}
