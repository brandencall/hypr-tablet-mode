#include "ipc/socket_client.h"

int connect_to_server() {
    std::string socket_path = "/tmp/tablet_ipc.sock";
    int sockfd;
    struct sockaddr_un addr;

    // Create socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    std::cout << "Connected to server" << '\n';
    return sockfd;
}

void handle_session(int socket, SocketEventCallback cb) {
    char buf[1024];
    ssize_t n;

    // Read messages in a loop
    while ((n = read(socket, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        cb(buf);
    }

    if (n < 0) {
        perror("read");
    } else {
        printf("Server closed connection\n");
    }

    close(socket);
}
