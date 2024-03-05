#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct sockaddr_in* createTCPServerAddress(char* input_ip, int port);  
int getUserInput(); 

int main(int argc, char *argv[]) {
    // Get ip and port from user defined options
    int opt;
    char* server_ip = NULL; 
    int port = -1; 
    while((opt = getopt(argc, argv, "s:p:")) != -1) {
        switch(opt) {
            case 's':
                server_ip = optarg;
                break; 
            case 'p':
                port = atoi(optarg);
                break; 
            default:
                perror("Invalid args");
                exit(1);
        }

    }
    if (server_ip == NULL || port == -1) {
        perror("Missing args"); 
        exit(1); 
    }
    
    printf("User Args -> \n"); 
    printf("Server Address: %s\n", server_ip); 
    printf("Server Port: %d\n", port); 

    // Create Client FD and server_addr for the connect function 
    int client_FD = socket(AF_INET, SOCK_STREAM, 0); 
    struct sockaddr_in* server_addr = createTCPServerAddress(server_ip, port);

    // Connect
    int connect_result = connect(client_FD, (struct sockaddr*) server_addr, sizeof(*server_addr)); 
    if (connect_result < 0) {
        perror("[Connect] Failed"); 
        exit(1); 
    } else {
        printf("Connected to the server (ip: %s, port: %d)\n", server_ip, port); 
    }

    // Get the client Port
    struct sockaddr_in client_addr; 
    socklen_t client_addr_size = sizeof(client_addr); 
    int getsocket_result = getsockname(client_FD, (struct sockaddr*) &client_addr, &client_addr_size); 
    if (getsocket_result == -1) {
        perror("[SOCKNAME] Failed"); 
        exit(1); 
    } else {
        printf("Client is using port: %d\n", ntohs(client_addr.sin_port)); 
    }


    // Get user input 
    int client_msg = getUserInput(); 
    
    // Send msg to server
    send(client_FD, &client_msg, sizeof(client_msg), 0); 

    // Receive message from server
    int server_msg; 
    int server_msg_size = recv(client_FD, &server_msg, sizeof(server_msg), 0); 
    if (server_msg_size > 0) {
        printf("Number from server is %d", server_msg); 
    } else if (server_msg_size) {
        printf("server_msg_size is 0"); 
    }

    // Close the socket
    close(client_FD); 

    
    return 0; 
}

struct sockaddr_in* createTCPServerAddress(char* input_ip, int port) {
    struct sockaddr_in* server_addr = malloc(sizeof(struct sockaddr_in)); 
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port); 
    inet_pton(AF_INET, input_ip, &server_addr->sin_addr.s_addr); 
    return server_addr; 
}

int getUserInput() {
    printf("Enter a number: \n");
    int server_msg; 
    scanf("%d", &server_msg);
    return server_msg; 
}
