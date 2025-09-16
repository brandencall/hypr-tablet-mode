#include "tablet.h"
#include "utils/hypr_util.h"
#include "utils/libinput_util.h"
#include "utils/sdbus_util.h"
#include <atomic>
#include <fcntl.h>
#include <iostream>
#include <libinput.h>
#include <poll.h>
#include <sys/poll.h>
#include <systemd/sd-bus.h>
#include <thread>
#include <unistd.h>

int main() {
    std::atomic<bool> tablet_mode{false};

    LibinputContextWrapper libinput_ctx = libinput_init();
    // Start the sdbus thread
    auto dbus_ctx = sdbus_init_accel_orient();
    std::thread dbus_thread(sdbus_start_processing_thread, dbus_ctx.bus, std::ref(tablet_mode));

    struct pollfd fds;
    fds.fd = libinput_ctx.fd;
    fds.events = POLLIN;

    while (true) {
        int ret = poll(&fds, 1, -1);
        if (ret > 0 && (fds.revents & POLLIN)) {
            libinput_poll(libinput_ctx, [&](bool tablet) {
                tablet_mode.store(tablet);
                std::cout << "Tablet mode: " << (tablet ? "ON" : "OFF") << "\n";
                if (tablet) {
                    enter_tablet_mode();
                } else {
                    exit_tablet_mode();
                }
            });
        }
    }

    libinput_cleanup(libinput_ctx);
    sdbus_cleanup(dbus_ctx);
    return 0;
}
