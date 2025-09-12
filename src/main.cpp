#include "utils/libinput_util.h"
#include <fcntl.h>
#include <iostream>
#include <libinput.h>
#include <poll.h>
#include <sys/poll.h>
#include <unistd.h>

int main() {
    auto libinput_ctx = libinput_init();

    while (true) {
        libinput_poll(libinput_ctx,
                      [](bool tablet) { std::cout << "Tablet mode: " << (tablet ? "ON" : "OFF") << "\n"; });
    }

    libinput_cleanup(libinput_ctx);
    return 0;
}
