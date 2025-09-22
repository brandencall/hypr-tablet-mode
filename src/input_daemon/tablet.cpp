#include "input_daemon/tablet.h"
#include "ipc/socket_server.h"

void handle_switch_event(libinput_event *e, int client_socket) {
    auto sw_event = libinput_event_get_switch_event(e);
    if (libinput_event_switch_get_switch(sw_event) == LIBINPUT_SWITCH_TABLET_MODE) {
        bool tablet_on = libinput_event_switch_get_switch_state(sw_event) == LIBINPUT_SWITCH_STATE_ON;
        if (tablet_on) {
            json msg = {{"type", "tablet"}, {"event", "tablet_on"}};
            if (!write_client(client_socket, msg.dump())) {
                std::cout << "failed to write to client" << '\n';
            }
        } else {
            json msg = {{"type", "tablet"}, {"event", "tablet_off"}};
            if (!write_client(client_socket, msg.dump())) {
                std::cout << "failed to write to client" << '\n';
            }
        }
    }
}
