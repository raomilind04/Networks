#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>

#define HTTP_PORT 80
#define BUFFER_SIZE 1024 

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Input Args are incorrect"); 
        exit(1); 
    }
    
    char *target_host_name = argv[1]; 
    int port = HTTP_PORT; 

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (socket_fd == -1) {
        perror("[Socket] Couldn't be created"); 
    }

    struct hostent *target_server = gethostbyname(target_host_name); 
    if (target_server == NULL) {
        perror("[Server] Couldn't resolve host by name"); 
        exit(1); 
    }

    struct sockaddr_in server_addr; 
    memset(&server_addr, 0, sizeof(server_addr)); 
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(port); 
    memcpy(&server_addr.sin_addr.s_addr, target_server->h_name, target_server->h_length); 


    if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
        perror("[Connect] Failed to connect to server");
        exit(1); 
    }

    char req[BUFFER_SIZE]; 
    snprintf(req, BUFFER_SIZE, "GET / HTTP/1.1\r\nHost: %s:%d\r\nConnection: close\r\n\r\n", target_host_name, port);

    if (send(socket_fd, req, strlen(req), 0) == -1) {
        perror("[Send] Unable to send message"); 
        exit(1); 
    }

    FILE *result_file = fopen("result.html", "wb"); 
    if (result_file == NULL) {
        perror("[fopen] Failure");
    }
    
    char buffer[BUFFER_SIZE]; 
    char prev_buffer[BUFFER_SIZE * 2]; 
    int bytes_read; 
    char *header_end; 
    bool header_read = false; 
    bool use_prev = false; 
    while ((bytes_read = recv(socket_fd, buffer, BUFFER_SIZE, 0)) > 0) {
        if (header_read) {
            fwrite(buffer, 1, bytes_read, result_file);
        } else {
            if (use_prev) {
                char merged_buffer[3 * BUFFER_SIZE]; 
                merged_buffer[0] = '\0'; 
                strncat(merged_buffer, prev_buffer, BUFFER_SIZE - 1); 
                strncat(merged_buffer, buffer, bytes_read); 
                header_end = strstr(merged_buffer, "\r\n\r\n"); 
                if (header_end != NULL) {
                    fwrite(header_end + 4, 1, bytes_read - (header_end - merged_buffer), result_file); 
                    header_read = true; 
                }
                strcpy(prev_buffer, buffer);
            } else {
                header_end = strstr(buffer, "\r\n\r\n"); 
                if (header_end != NULL) {
                    fwrite(header_end + 4, 1, bytes_read - (header_end - buffer), result_file); 
                    header_read = true; 
                }
                use_prev = true; 
            }
        }
    }

    fclose(result_file); 
    close(socket_fd); 

    if (bytes_read == -1) {
        perror("[READ] Unable to read");
        exit(1); 
    }

    printf("Result saved in result.html file"); 
    exit(0); 
}
