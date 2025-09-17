#include "utils/libinput_util.h"
#include "utils/sdbus_util.h"
#include <fcntl.h>
#include <libinput.h>
#include <poll.h>
#include <sys/poll.h>
#include <systemd/sd-bus.h>
#include <thread>
#include <unistd.h>

int main() {
    LibinputContextWrapper libinput_ctx = libinput_init();
    // Start the sdbus thread
    auto dbus_ctx = sdbus_init_accel_orient();
    std::thread dbus_thread(sdbus_start_processing_thread, dbus_ctx.bus);

    struct pollfd fds;
    fds.fd = libinput_ctx.fd;
    fds.events = POLLIN;

    while (true) {
        int ret = poll(&fds, 1, -1);
        if (ret > 0 && (fds.revents & POLLIN)) {
            libinput_poll(libinput_ctx);
        }
    }

    libinput_cleanup(libinput_ctx);
    sdbus_cleanup(dbus_ctx);
    return 0;
}
