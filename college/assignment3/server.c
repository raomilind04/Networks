#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct sockaddr_in* createTCPServerAddress(char* input_ip, int port); 
void handleClient(int client_FD, struct sockaddr_in client);
int getUserInput(); 

int main(int argc, char *argv[]) {
    // Get ip and port from user defined options
    int opt;
    char* input_ip= NULL; 
    int port = -1; 
    while((opt = getopt(argc, argv, "i:p:")) != -1) {
        switch(opt) {
            case 'i':
                input_ip = optarg;
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
    if (input_ip == NULL || port == -1) {
        perror("Missing args"); 
        exit(1); 
    }

    printf("Address: %s\n", input_ip); 
    printf("Port: %d\n", port); 
   
    //Create file descriptor and server adder struct
    int server_FD = socket(AF_INET, SOCK_STREAM, 0); 
    struct sockaddr_in* server_addr = createTCPServerAddress(input_ip, port);

    // Bind
    int binding_result = bind(server_FD, (struct sockaddr*) server_addr, sizeof(*server_addr)); 
    if (binding_result == 0) {
        printf("[Binding Completed] ip: %s | port: %d\n", input_ip, port); 
    } else {
        perror("[Binding] Failed\n"); 
    }

    // Listen
    int listen_result = listen(server_FD, 10); 
    if (listen_result == 0) {
        printf("[Listen] Listening...\n"); 
    } else {
        perror("[Listen] Failed\n"); 
    }


    // Accept 
    while (1) {
        struct sockaddr_in client_addr;  
        socklen_t client_addr_size = sizeof(struct sockaddr_in); 
        int client_FD = accept(server_FD, (struct sockaddr*) &client_addr, &client_addr_size); 
        if (client_FD <= 0) {
            perror("[Accept] Unable to accept client"); 
        }
        
        // Receive  
        handleClient(client_FD, client_addr); 
    }

    // shutdown the server
    shutdown(server_FD, SHUT_RDWR);
    
    return 0; 
}

struct sockaddr_in* createTCPServerAddress(char* input_ip, int port) {
    struct sockaddr_in* server_addr = malloc(sizeof(struct sockaddr_in)); 
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port); 
    inet_pton(AF_INET, input_ip, &server_addr->sin_addr.s_addr); 
    return server_addr; 
}

void handleClient(int client_FD, struct sockaddr_in client) {
    int client_msg; 
    ssize_t client_msg_size = recv(client_FD, &client_msg, sizeof(client_msg), 0);

    if (client_msg_size > 0) {
        // Receive client_msg
        printf("Number from client is %d\n", client_msg);
        
        // Get user input at server end
        int server_msg = getUserInput() + client_msg; 

        // Send sum of server_msg and client_msg back
        send(client_FD, &server_msg, sizeof(server_msg), 0); 
    } else if (client_msg_size == 0) {
       printf("client_msg_size was 0\n");  
    }

    // Close the client FD
    close(client_FD); 
}

int getUserInput() {
    printf("Enter a number: \n");
    int server_msg; 
    scanf("%d", &server_msg);
    return server_msg; 
}
