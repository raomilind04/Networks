#include "utils.h" 
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

void createThreadToListen(int socketFD); 
void listenForServerEvents(int socketFD); 
void readAndSendToServer(int socketFD); 

int main() {
  int socketFD = createTCPSocket(); 
  struct sockaddr_in* addr = createAddress("127.0.0.1", 2000); 

  int result = connect(socketFD, addr, sizeof(*addr));
  if (result == 0) printf("Connected to the server \n"); 

  createThreadToListen(socketFD); 
  readAndSendToServer(socketFD); 

  close(socketFD); 

  return 0; 
}

void createThreadToListen(int socketFD) {
  pthread_t id; 
  pthread_create(&id, NULL, listenForServerEvents, socketFD); 
}

void listenForServerEvents(int socketFD) {
  char buffer[1024]; 
  while (true) {
    ssize_t dataSize = recv(socketFD, buffer, 1024, 0); 

    if (dataSize > 0) {
      buffer[dataSize] = 0; 
      printf("%s\n", buffer); 
    }

    if (dataSize == 0) break;  
  }

  close(socketFD); 
}

void readAndSendToServer(int socketFD) {
  char* name = NULL; 
  size_t nameSize = 0; 
  printf("Enter you name: \n"); 

  ssize_t nameCount = getline(&name, &nameSize, stdin); 
  name[nameCount-1] = 0; 

  char* line = NULL; 
  size_t lineSize = 0; 
  printf("Enter your message\n"); 

  char buffer[1024]; 
  
  while (true) {
    ssize_t charCount = getline(&line, &lineSize, stdin); 
    line[charCount-1] = 0; 
    sprintf(buffer, "%s:%s", name, line); 

    if (charCount > 0) {
      if (strcmp(line, "exit") == 0) {
        break; 
      }
      ssize_t dataSent = send(socketFD, buffer, strlen(buffer), 0); 
    }
  }
}
