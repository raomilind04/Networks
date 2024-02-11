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
        //wait(NULL); 
        waitpid(first_fork, NULL, 0);
        printf("Parent/pid:%d says Child/pid:%d has exited\n", getpid(), first_fork); 
        sleep(1); 
        printf("Parent/pid:%d is exiting\n", getpid());
        exit(0); 
    } else {
        // This is the child process
        printf("Child/pid:%d created (parent process pid:%d)\n", getpid(), getppid()); 
        pid_t second_fork = fork(); 

        if (second_fork == -1) {
            printf("Forked Failed\n");
            exit(1); 
        } else if (second_fork > 0) {
            // This is the child process (i.e. Parent after the second fork)
            
            //wait(NULL); 
            waitpid(second_fork, NULL, 0);
            printf("Child/pid:%d says GrandChild/pid:%d has exited\n", getpid(), second_fork); 
            sleep(1); 
            exit(0); 
        } else {
            printf("GrandChild/pid:%d created (parent process pid:%d)\n", getpid(), getppid());
            sleep(1);
            exit(0); 
        }
    }
}
