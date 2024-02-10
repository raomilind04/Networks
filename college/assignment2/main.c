#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define READ 0
#define WRITE 1

int main() {

    // Create a pipe for Parent-Child Communication
    int parent_child_fd[2];
    int child_parent_fd[2]; 
    if (pipe(parent_child_fd) == -1) {
        perror("[Parent] Unable to create a pipe");
        exit(1); 
    }
    if (pipe(child_parent_fd) == -1) {
        perror("[Parent] Unable to create a pipe"); 
    }
    
    // Fork Parent process to create Child
    pid_t first_fork = fork(); 

    if (first_fork == -1) {
        perror("First fork failed");
        exit(1);
    } else if (first_fork > 0) {
        // This is the parent process
        
        // Print pid for the parent
        printf("Parent/pid:%d created\n", getpid()); 

        // Get input for parent
        int parent_input; 
        printf("Integer input for parent: ");
        scanf("%d", &parent_input); 

        // Send input number to the child
        close(parent_child_fd[READ]);
        if (write(parent_child_fd[WRITE], &parent_input, sizeof(int)) == -1) {
            perror("[Parent] Unable to write"); 
            exit(1); 
        } 
        close(parent_child_fd[WRITE]); 

        // Read number from child
        int child_parent; 
        close(child_parent_fd[WRITE]);
        if (read(child_parent_fd[READ], &child_parent, sizeof(int)) == -1) {
            perror("[Parent] Unable to read"); 
            exit(1); 
        }
        close(child_parent_fd[READ]); 
        
        // Print result from child
        printf("[Parent/%d] Child sent: %d\n", getpid(), child_parent);

        // Parent exiting
        waitpid(first_fork, NULL, 0);
        printf("[Parent/%d] Exiting\n", getpid());
        exit(0); 
    } else {
        // This is the child process
        
        // Create pipe for Child-GrandChild Communication
        int child_grandchild_fd[2];
        int grandchild_child_fd[2]; 
        if (pipe(child_grandchild_fd) == -1) {
            perror("[Child] Unable to create a pipe"); 
        }
        if (pipe(grandchild_child_fd) == -1) {
            perror("[Child] Unable to create a pipe"); 
        }

        // Fork child process to create GrandChild
        pid_t second_fork = fork(); 

        if (second_fork == -1) {
            perror("Second Fork failed"); 
            exit(1); 
        } else if (second_fork > 0) {
            // This is the child process (i.e. Parent after the second fork)
           
            // Read input from parent
            int parent_child; 
            close(parent_child_fd[WRITE]); 
            if (read(parent_child_fd[READ], &parent_child, sizeof(int)) == -1) {
                perror("[Child] Unable to read"); 
                exit(1); 
            }
            close(parent_child_fd[READ]); 
            
            // Print pid for child
            printf("Child/pid:%d created (parent process pid:%d)\n", getpid(), getppid()); 

            // Print input from parent
            printf("[Child/%d] Parent sent: %d\n", getpid(), parent_child); 

            // Get input for child
            int child_input; 
            printf("Integer input for child: "); 
            scanf("%d", &child_input); 

            // Send child_input + parent_child to GrandChild
            int child_grandchild_result = child_input + parent_child; 
            close(child_grandchild_fd[READ]); 
            if (write(child_grandchild_fd[WRITE], &child_grandchild_result, sizeof(int)) == -1) {
                perror("[Child] Unable to write"); 
            }
            close(child_grandchild_fd[WRITE]); 

            // Read number from GrandChild
            int grandchild_child; 
            close(grandchild_child_fd[WRITE]);
            if (read(grandchild_child_fd[READ], &grandchild_child, sizeof(int)) == -1) {
                perror("[Child] Unable to read"); 
                exit(1);
            }
            close(grandchild_child_fd[READ]); 

            // Print number from GrandChild
            printf("[Child/%d] GrandChild sent: %d\n", getpid(), grandchild_child); 

            // Send grandchild_child to Parent
            int child_parent_result = grandchild_child; 
            close(child_parent_fd[READ]); 
            if (write(child_parent_fd[WRITE], &child_parent_result, sizeof(int)) == -1) {
                perror("[Child] Unable to write"); 
            }
            close(child_parent_fd[WRITE]);
    
            // Child exiting
            waitpid(second_fork, NULL, 0);
            printf("[Child/%d] Exiting\n", getpid()); 
            exit(0); 
        } else {
           
            // Read input from child
            int child_grandchild; 
            close(child_grandchild_fd[WRITE]); 
            if (read(child_grandchild_fd[READ], &child_grandchild, sizeof(int)) == -1) {
                perror("[GrandChild] Unable to read"); 
            }
            close(child_grandchild_fd[READ]); 

            // This is the GrandChild process
            printf("GrandChild/pid:%d created (parent process pid:%d)\n", getpid(), getppid());

            // Print input from child
            printf("[GrandChild/%d] Child sent: %d\n", getpid(), child_grandchild); 

            // Get input for GrandChild
            int grandchild_input; 
            printf("Integer input for GrandChild: "); 
            scanf("%d", &grandchild_input); 

            // Send grandchild_input + child_grandchild to the Child
            int grandchild_result = grandchild_input + child_grandchild; 
            close(grandchild_child_fd[READ]);
            if (write(grandchild_child_fd[WRITE], &grandchild_result, sizeof(int)) == -1) {
                perror("[GrandChild] Unable to write"); 
                exit(1); 
            }
            close(grandchild_child_fd[WRITE]); 
            
            // GrandChild exiting
            printf("[GrandChild/%d] Exiting \n", getpid()); 
            exit(0); 
        }
    }
}
