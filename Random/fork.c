#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
  int pid = fork(); 
  switch (pid) {
    case -1: 
      printf("Could not fork the process \n"); 
      exit(1); 
    case 0:
      printf("This is the child process \n"); 
      break; 
    default:
      printf("This is the parent process \n"); 
      break; 
  }
  return 0;
}
