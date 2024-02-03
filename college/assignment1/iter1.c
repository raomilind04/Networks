#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
    pid_t first_fork = fork(); 

    if (first_fork == -1) {
        printf("Fork Failed\n");
        exit(1);
    } else if (first_fork > 0) {
        // This is the parent process
        
        printf("Parent/pid:%d created\n", getpid()); 
        wait(NULL); 
        printf("Parent says Child/pid:%d has exited\n", first_fork); 
        printf("Parent/pid:%d is exiting\n", getpid());
    } else {
        // This is the child process
        printf("Child/pid:%d created\n", getpid()); 
        pid_t second_fork = fork(); 

        if (second_fork == -1) {
            printf("Forked Failed\n");
            exit(1); 
        } else if (second_fork > 0) {
            // This is the child process (i.e. Parent after the second fork)
            wait(NULL);
            printf("Child says GrandChild/pid:%d has exited\n", second_fork); 
            exit(0); 
        } else {
            printf("GrandChild/pid:%d created\n", getpid());
            exit(0); 
        }
    }
}
