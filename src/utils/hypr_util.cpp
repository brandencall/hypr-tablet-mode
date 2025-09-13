#include "utils/hypr_util.h"
#include <poll.h>

int connect_to_hypr() {
    const char *xdg = std::getenv("XDG_RUNTIME_DIR");
    const char *hypr_sig = std::getenv("HYPRLAND_INSTANCE_SIGNATURE");
    std::string sig = std::string(xdg) + "/hypr/" + std::string(hypr_sig) + "/.socket.sock";

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return fd;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, sig.c_str(), sizeof(addr.sun_path) - 1);

    int connection = connect(fd, (sockaddr *)&addr, sizeof(addr));

    if (connection < 0) {
        perror("connect");
        close(fd);
        return connection;
    }

    return fd;
}

int execute_hypr_cmd(std::string cmd) {
    int fd = connect_to_hypr();
    std::cout << "writing command: " << cmd << '\n';

    int write_cmd = write(fd, cmd.c_str(), cmd.size());
    if (write_cmd < 0) {
        perror("write");
        return write_cmd;
    }

    char buffer[1024];
    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    char ok_resp[] = "ok";
    if (n > 0) {
        buffer[n] = '\0';
        std::cout << "Response: " << buffer << "\n";
        if (strcmp(ok_resp, buffer) == 0) {
            std::cout << "Good response" << '\n';
            return 1;
        } else {
            std::cout << "Bad response" << '\n';
            return -1;
        }
    }

    return 1;
}

