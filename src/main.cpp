#include "utils/libinput_util.h"
#include "utils/sdbus_util.h"
#include <atomic>
#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <libinput.h>
#include <poll.h>
#include <sys/poll.h>
#include <systemd/sd-bus.h>
#include <thread>
#include <unistd.h>

std::atomic<bool> tablet_mode{false};

void dbus_loop(sd_bus *bus, std::atomic<bool> &tablet_mode) {
    while (true) {
        if (!tablet_mode.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }
        int r = sd_bus_process(bus, NULL);
        if (r == 0) {
            sd_bus_wait(bus, (uint64_t)-1);
        }
    }
}

int main() {
    LibinputContextWrapper libinput_ctx = libinput_init();

    auto dbus_ctx = sdbus_init();
    std::thread dbus_thread(dbus_loop, dbus_ctx.bus, std::ref(tablet_mode));

    struct pollfd fds;
    fds.fd = libinput_ctx.fd;
    fds.events = POLLIN;

    while (true) {
        int ret = poll(&fds, 1, -1);
        if (ret > 0 && (fds.revents & POLLIN)) {
            libinput_poll(libinput_ctx, [&](bool tablet) {
                tablet_mode.store(tablet);
                std::cout << "Tablet mode: " << (tablet ? "ON" : "OFF") << "\n";
            });
        }
    }

    libinput_cleanup(libinput_ctx);
    sdbus_cleanup(dbus_ctx);
    return 0;
}
