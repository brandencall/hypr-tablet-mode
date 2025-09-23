#pragma once
#include <atomic>
#include <functional>
#include <systemd/sd-bus.h>

using LidCallback = std::function<void(bool closed)>;

struct SDBusWrapper {
    sd_bus *bus;
    sd_bus_slot *slot;
};
struct Context {
    int client_socket;
};

SDBusWrapper sdbus_init_accel_orient(int client_socket);
void sdbus_start_processing_thread(sd_bus *bus, std::atomic<bool> *sdbus_running);
void sdbus_cleanup(SDBusWrapper &busWrapper);
