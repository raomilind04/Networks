#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int opt;
    char* serverAddress = NULL; 
    int port = -1; 
    while((opt = getopt(argc, argv, "i:p:")) != -1) {
        switch(opt) {
            case 'i':
                serverAddress = optarg;
                break; 
            case 'p':
                port = strtol(optarg, NULL, 10);
                if (port <= 0 || port > 65535) {
                    perror("Invalid port number"); 
                    exit(1);
                }
                break; 
            default:
                perror("Invalid args");
                exit(1);
        }

    }
    if (serverAddress == NULL || port == -1) {
        perror("Missing args"); 
        exit(1); 
    }

    printf("Address: %s\n", serverAddress); 
    printf("Port: %d\n", port); 

    return 0; 
}
