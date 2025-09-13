#pragma once
#include <systemd/sd-bus.h>
#include <functional>
#include <atomic>

using LidCallback = std::function<void(bool closed)>;

struct SDBusWrapper {
    sd_bus *bus;
    sd_bus_slot *slot;
};

SDBusWrapper sdbus_init_accel_orient();
void sdbus_start_processing_thread(sd_bus *bus, std::atomic<bool> &tablet_mode);
void sdbus_cleanup(SDBusWrapper &busWrapper);
