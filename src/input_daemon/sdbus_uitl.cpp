#include "input_daemon/sdbus_util.h"
#include "ipc/socket_server.h"
#include <cstdio>
#include <iostream>
#include <unistd.h>

static int property_changed_handler(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
    Context *ctx = static_cast<Context *>(userdata);
    int client_socket = ctx->client_socket;
    const char *interface;
    int r;

    // First argument: interface name
    r = sd_bus_message_read(m, "s", &interface);
    if (r < 0) {
        fprintf(stderr, "Failed to read interface: %s\n", strerror(-r));
        return r;
    }

    // Second argument: a{sv} dict of changed properties
    r = sd_bus_message_enter_container(m, 'a', "{sv}");
    if (r < 0)
        return r;

    while ((r = sd_bus_message_enter_container(m, 'e', "sv")) > 0) {
        const char *prop_name;
        r = sd_bus_message_read(m, "s", &prop_name);
        if (r < 0)
            return r;
        if (strcmp(prop_name, "AccelerometerOrientation") == 0) {
            const char *orientation;

            r = sd_bus_message_enter_container(m, 'v', "s");
            if (r < 0)
                return r;
            r = sd_bus_message_read(m, "s", &orientation);
            if (r < 0)
                return r;
            r = sd_bus_message_exit_container(m);
            if (r < 0)
                return r;

            // TODO: implement the ipc socket write for orientation change
            printf("Orientation changed: %s\n", orientation);
            json msg = {{"type", "tablet"}, {"event", orientation}};
            if (!write_client(client_socket, msg.dump())) {
                std::cout << "failed to write to client" << '\n';
            }
        } else {
            // Skip other properties
            r = sd_bus_message_skip(m, "v");
            if (r < 0)
                return r;
        }

        sd_bus_message_exit_container(m); // 'e'
    }
    sd_bus_message_exit_container(m); // 'a'

    return 0;
}

SDBusWrapper sdbus_init_accel_orient(int client_socket) {
    sd_bus *bus = NULL;
    sd_bus_slot *slot = NULL;
    int r;

    Context *ctx = new Context{client_socket};

    // Connect to the system bus
    r = sd_bus_open_system(&bus);
    if (r < 0) {
        std::cerr << "Failed to connect to system bus" << '\n';
    }

    r = sd_bus_call_method(bus,
                           "net.hadess.SensorProxy",  // service
                           "/net/hadess/SensorProxy", // object path
                           "net.hadess.SensorProxy",  // interface
                           "ClaimAccelerometer",      // method
                           NULL,                      // no error
                           NULL,                      // no reply needed
                           NULL);
    if (r < 0) {
        std::cerr << "Failed to claim accelerometer" << '\n';
    }

    // Match properties changed on /net/hadess/SensorProxy
    r = sd_bus_match_signal(bus, &slot,
                            "net.hadess.SensorProxy",          // Sender
                            "/net/hadess/SensorProxy",         // Object path
                            "org.freedesktop.DBus.Properties", // Interface
                            "PropertiesChanged",               // Member
                            property_changed_handler, ctx);
    if (r < 0) {
        std::cerr << "Failed to add match" << '\n';
    }
    return {bus, slot};
}

void sdbus_start_processing_thread(sd_bus *bus, std::atomic<bool> *sdbus_running) {
    while (sdbus_running->load()) {
        int r = sd_bus_process(bus, NULL);
        // TODO: what to do if sd_bus process fails
        if (r < 0) {
            std::cerr << "err in the sdbus thread" << '\n';
            break;
        }
        if (r == 0) {
            sd_bus_wait(bus, 200 * 1000 * 1000);
        }
    }
}

void sdbus_cleanup(SDBusWrapper &busWrapper) {
    sd_bus_slot_unref(busWrapper.slot);
    sd_bus_unref(busWrapper.bus);
}
