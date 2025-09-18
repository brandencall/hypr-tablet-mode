#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/tablet_ipc.sock" // adjust to match your server

int main(void) {
    int sockfd;
    struct sockaddr_un addr;
    char buf[1024];
    ssize_t n;

    // Create socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s\n", SOCKET_PATH);

    // Read messages in a loop
    while ((n = read(sockfd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("Received: %s\n", buf);
    }

    if (n < 0) {
        perror("read");
    } else {
        printf("Server closed connection\n");
    }

    close(sockfd);
    return 0;
}
