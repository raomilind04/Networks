#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

char* msg = "Hello World";

int main() {
  int fd[2]; 
  pipe(fd);

  char buffer[100]; 

  if (fork() == 0) {
    // child 1
    close(fd[READ]);
    write(fd[WRITE], msg, strlen(msg)+1); 
    close(fd[WRITE]); 
  } else {
    // parent 
    wait(NULL); 
    printf("Waiting for first child\n"); 
    if (fork() == 0) {
      // child 2
      close(fd[WRITE]); 
      int readBytes = read(fd[READ], buffer, sizeof(buffer)); 
      printf("Child : Read %d bytes : %s\n", readBytes, buffer); 
      close(fd[READ]); 
    } else {
      printf("Waiting for second child to finish reading\n"); 
      wait(NULL); 
      close(fd[WRITE]); 
      int readBytes = read(fd[READ], buffer, sizeof(buffer)); 
      printf("Parent : Read %d bytes : %s\n", readBytes, buffer); 
      close(fd[READ]); 
    }
  }
}
