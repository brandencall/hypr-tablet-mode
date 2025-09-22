#include "ipc/socket_server.h"
#include "input_daemon/libinput_util.h"
#include "input_daemon/sdbus_util.h"
#include <cstdio>
#include <iostream>

int create_server_socket() {
    const char *SOCKET_PATH = "/tmp/tablet_ipc.sock";

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH); // remove old socket
    if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind failed");
        close(server_fd);
    }

    // TODO: NEED TO CHANGE THIS
    // change owner to your user
    struct passwd *pw = getpwnam("brabs");
    if (pw) {
        if (chown("/tmp/tablet_ipc.sock", pw->pw_uid, pw->pw_gid) == -1) {
            perror("chown");
        }
    }

    // change permissions so user can read/write
    if (chmod("/tmp/tablet_ipc.sock", 0660) == -1) {
        perror("chmod");
    }
    listen(server_fd, 5);

    if (listen(server_fd, 5) == -1) {
        std::cout << "Error on listen" << std::endl;
        close(server_fd);
        return -1;
    }

    return server_fd;
}

void client_session(int client_socket) {

    LibinputContextWrapper libinput_ctx = libinput_init();
    // Start the sdbus thread
    auto dbus_ctx = sdbus_init_accel_orient();
    std::thread dbus_thread(sdbus_start_processing_thread, dbus_ctx.bus);
    dbus_thread.detach();

    struct pollfd fds[2];
    fds[0].fd = client_socket;
    fds[0].events = POLLIN | POLLHUP | POLLERR;
    fds[1].fd = libinput_ctx.fd;
    fds[1].events = POLLIN;

    while (true) {
        int ret = poll(fds, 2, -1);

        if (ret < 0) {
            perror("poll failed");
            break;
        }

        if (fds[0].revents & (POLLHUP | POLLERR)) {
            std::cout << "client disconnected" << '\n';
            break;
        }

        if (fds[1].revents & POLLIN) {
            int write_client = libinput_poll(libinput_ctx, client_socket);
            if (write_client <= 0) {
                std::cout << "Write to client was <= 0" << '\n';
                break;
            }
        }
    }

    close(client_socket);
    libinput_cleanup(libinput_ctx);
    sdbus_cleanup(dbus_ctx);
}

bool write_client(int client_socket, const std::string &msg) {
    ssize_t n = write(client_socket, msg.c_str(), msg.size());
    if (n < 0) {
        return false;
    }
    return true;
}
