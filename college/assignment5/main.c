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
        exit(1);
    }

    struct addrinfo hints, *result, *rp;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(target_host_name, NULL, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "[Server] Couldn't resolve host by name: %s\n", gai_strerror(status));
        exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_family == AF_INET) {
            struct sockaddr_in *ipv4_addr = (struct sockaddr_in *)rp->ai_addr;
            memcpy(&server_addr.sin_addr, &ipv4_addr->sin_addr, sizeof(struct in_addr));
            break; 
        }
    }

    freeaddrinfo(result);

    if (rp == NULL) {
        perror("[Connect] No suitable address found");
        exit(1);
    }

    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("[Connect] Failed to connect to server");
        exit(1);
    }

    char req[BUFFER_SIZE];


    snprintf(req, BUFFER_SIZE,
             "GET / HTTP/1.1\r\n"
             "Host: %s:%d\r\n"
             "User-Agent: MyHTTPClient\r\n"
             "Accept: text/html\r\n"
             "Connection: close\r\n\r\n",
             target_host_name, port);

    if (send(socket_fd, req, strlen(req), 0) == -1) {
        perror("[Send] Unable to send message");
        exit(1);
    }

    FILE *result_file = fopen("index.html", "wb");
    if (result_file == NULL) {
        perror("[fopen] Failure");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    int bytes_read;
    bool header_read = false;

    while ((bytes_read = recv(socket_fd, buffer, BUFFER_SIZE, 0)) > 0) {
        if (!header_read) {
            char *header_end = strstr(buffer, "\r\n\r\n");
            if (header_end != NULL) {
                fwrite(header_end + 4, 1, bytes_read - (header_end - buffer), result_file);
                header_read = true;
            }
        } else {
            fwrite(buffer, 1, bytes_read, result_file);
        }
    }

    fclose(result_file);
    close(socket_fd);

    if (bytes_read == -1) {
        perror("[READ] Unable to read");
        exit(1);
    }

    printf("Result saved in index.html file\n");
    exit(0);
}
