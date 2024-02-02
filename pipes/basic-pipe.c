#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define READ 0 
#define WRITE 1

char* msg = "Hello World"; 

int main() {
  int fd[2];
  char buffer[100]; 
  pipe(fd); 

  if (fork() == 0) {
    close(fd[READ]);
    write(fd[WRITE], msg, strlen(msg)+1);  
    printf("Size of the msg is %d\n", sizeof(msg)); 
    printf("Msg : %s\n", msg); 
    close(fd[WRITE]); 
  } else {
    close(fd[WRITE]);
    int bytesRead = read(fd[READ], buffer, sizeof(buffer));
    printf("Size of the buffer is %d\n", sizeof(buffer)); 
    printf("Read %d bytes : %s\n", bytesRead, buffer); 
    close(fd[READ]); 
  }

  return 0; 
}
