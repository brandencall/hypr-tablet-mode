#include "ipc/socket_server.h"
#include <iostream>

int main() {
    int server_socket = create_server_socket();

    if (server_socket == -1)
        return -1;
    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        std::cout << "client connected" << '\n';
        if (client_socket == -1)
            break;
        std::thread(client_session, client_socket).detach();
    }
    close(server_socket);

    return 0;
}
