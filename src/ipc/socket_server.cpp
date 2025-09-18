#include "ipc/socket_server.h"
#include "input_daemon/libinput_util.h"
#include "input_daemon/sdbus_util.h"
#include <mutex>
#include <unordered_map>

std::unordered_map<int, std::mutex> client_mutexes;
std::mutex map_mutex;

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
    listen(server_fd, 5);

    if (listen(server_fd, 5) == -1) {
        std::cout << "Error on listen" << std::endl;
        close(server_fd);
        return -1;
    }

    return server_fd;
}

void client_session(int client_socket) {
    {
        std::lock_guard<std::mutex> lock(map_mutex);
        client_mutexes[client_socket];
    }

    LibinputContextWrapper libinput_ctx = libinput_init();
    // Start the sdbus thread
    auto dbus_ctx = sdbus_init_accel_orient();
    std::thread dbus_thread(sdbus_start_processing_thread, dbus_ctx.bus);
    dbus_thread.detach();

    struct pollfd fds;
    fds.fd = libinput_ctx.fd;
    fds.events = POLLIN;

    bool running = true;
    while (running) {
        int ret = poll(&fds, 1, -1);
        if (ret > 0 && (fds.revents & POLLIN)) {
            int write_client = libinput_poll(libinput_ctx, client_socket);
            if (write_client <= 0) {
                running = false;
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(map_mutex);
        client_mutexes.erase(client_socket);
    }

    close(client_socket);
    libinput_cleanup(libinput_ctx);
    sdbus_cleanup(dbus_ctx);
}

bool write_client(int client_socket, const std::string &msg) {
    std::lock_guard<std::mutex> lock(client_mutexes[client_socket]);
    ssize_t n = write(client_socket, msg.c_str(), msg.size());
    if (n < 0) {
        return false;
    }
    return true;
}
