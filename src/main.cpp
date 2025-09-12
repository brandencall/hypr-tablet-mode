#include "utils/libinput_util.h"
#include "utils/sdbus_util.h"
#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <libinput.h>
#include <poll.h>
#include <sys/poll.h>
#include <systemd/sd-bus.h>
#include <unistd.h>

void handle_tablet_mode(bool tablet, sd_bus *bus) {
    while (true) {
        int r = sd_bus_process(bus, NULL);
        if (r < 0) {
            fprintf(stderr, "Failed to process bus: %s\n", strerror(-r));
            break;
        }
        if (r > 0)
            continue; // we processed a request, try again immediately

        // Wait for the next event
        r = sd_bus_wait(bus, (uint64_t)-1);
        if (r < 0) {
            fprintf(stderr, "Failed to wait on bus: %s\n", strerror(-r));
            break;
        }
    }
}

int main() {
    LibinputContextWrapper libinput_ctx = libinput_init();

    auto dbus_ctx = sdbus_init([](bool tablet) { std::cout << "SDBUS DETECTED SOMETHING" << "\n"; });

    struct pollfd fds;
    fds.fd = libinput_ctx.fd;
    fds.events = POLLIN;

    while (true) {
        int ret = poll(&fds, 1, -1);
        if (ret > 0 && (fds.revents & POLLIN)) {
            libinput_poll(libinput_ctx, [dbus_ctx](bool tablet) {
                std::cout << "Tablet mode: " << (tablet ? "ON" : "OFF") << "\n";
                while (true) {
                    int r = sd_bus_process(dbus_ctx.bus, NULL);
                    if (r < 0) {
                        fprintf(stderr, "Failed to process bus: %s\n", strerror(-r));
                        break;
                    }
                    if (r > 0)
                        continue; // we processed a request, try again immediately

                    // Wait for the next event
                    r = sd_bus_wait(dbus_ctx.bus, (uint64_t)-1);
                    if (r < 0) {
                        fprintf(stderr, "Failed to wait on bus: %s\n", strerror(-r));
                        break;
                    }
                }
            });
        }
    }

    libinput_cleanup(libinput_ctx);
    sdbus_cleanup(dbus_ctx);
    return 0;
}
