#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

struct sockaddr_in* createTCPServerAddress(char* input_ip, int port);
void* handleClient(void* arg);
int getUserInput();

typedef struct {
    int client_FD;
    struct sockaddr_in client_addr;
} client_info_t;

int server_constant;
int connected_clients_count = 0; 

int main(int argc, char *argv[]) {
    // Get ip and port from user defined options
    int opt;
    char* input_ip = NULL;
    int port = -1;
    while ((opt = getopt(argc, argv, "i:p:c:")) != -1) {
        switch (opt) {
            case 'i':
                input_ip = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'c':
                server_constant = atoi(optarg); 
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

    printf("User Args -> \n");
    printf("Server Address: %s\n", input_ip);
    printf("Server Port: %d\n", port);

    // Create file descriptor and server adder struct
    int server_FD = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in* server_addr = createTCPServerAddress(input_ip, port);

    // Bind
    int binding_result = bind(server_FD, (struct sockaddr*)server_addr, sizeof(*server_addr));
    if (binding_result == 0) {
        printf("[Binding Completed] ip: %s | port: %d\n", input_ip, port);
    } else {
        perror("[Binding] Failed\n");
    }

    // Listen
    int listen_result = listen(server_FD, 2);
    if (listen_result == 0) {
        printf("[Listen] Listening...\n");
    } else {
        perror("[Listen] Failed\n");
    }

    // Accept
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(struct sockaddr_in);
        int client_FD = accept(server_FD, (struct sockaddr*)&client_addr, &client_addr_size);
        if (client_FD <= 0) {
            perror("[Accept] Unable to accept client");
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("New client connected from %s:%d\n", client_ip, ntohs(client_addr.sin_port));
        connected_clients_count++; 
        printf("%d clients are connected to the server\n", connected_clients_count);

        // Create a new thread to handle the client
        client_info_t* client_info = malloc(sizeof(client_info_t));
        client_info->client_FD = client_FD;
        client_info->client_addr = client_addr;
        pthread_t thread;
        pthread_create(&thread, NULL, handleClient, (void*)client_info);
        pthread_detach(thread);
    }

    // Shutdown the server
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

void* handleClient(void* arg) {
    client_info_t* client_info = (client_info_t*)arg;
    int client_FD = client_info->client_FD;
    struct sockaddr_in client = client_info->client_addr;
    free(client_info); // Free the client_info struct
    
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client.sin_addr), client_ip, INET_ADDRSTRLEN); 

    int client_msg;
    ssize_t client_msg_size = recv(client_FD, &client_msg, sizeof(client_msg), 0);
    if (client_msg_size > 0) {
        // Receive client_msg
        printf("Number from client (%s:%d) is %d\n", client_ip, ntohs(client.sin_port), client_msg);
        // Get user input at server end
        int server_msg = server_constant + client_msg;
        // Send sum of server_msg and client_msg back
        send(client_FD, &server_msg, sizeof(server_msg), 0);
    } else if (client_msg_size == 0) {
        printf("client_msg_size was 0\n");
    }

    // Close the client FD
    connected_clients_count--; 
    close(client_FD);
    pthread_exit(NULL);
}

