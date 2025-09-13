#pragma once
#include <systemd/sd-bus.h>
#include <functional>

using LidCallback = std::function<void(bool closed)>;

struct SDBusWrapper {
    sd_bus *bus;
    sd_bus_slot *slot;
};

SDBusWrapper sdbus_init();
void sdbus_cleanup(SDBusWrapper &busWrapper);
