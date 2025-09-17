#include "tablet.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

void execute_script(std::filesystem::path script) {
    if (fork() == 0) {
        execl(script.c_str(), script.c_str(), (char *)nullptr);
        _exit(127);
    }
}

void handle_switch_event(libinput_event *e) {
    auto sw_event = libinput_event_get_switch_event(e);
    if (libinput_event_switch_get_switch(sw_event) == LIBINPUT_SWITCH_TABLET_MODE) {
        bool tablet_on = libinput_event_switch_get_switch_state(sw_event) == LIBINPUT_SWITCH_STATE_ON;
        if (tablet_on) {
            enter_tablet_mode();
        } else {
            exit_tablet_mode();
        }
    }
}

void enter_tablet_mode() {
    int hypr_write = execute_hypr_cmd("dispatch workspace empty");
    const char *home = std::getenv("HOME");
    if (!home) {
        std::cout << "there is no home directory" << '\n';
        return;
    }
    std::filesystem::path script = std::filesystem::path(home) / ".config/eww/launch_dashboard.sh";
    execute_script(script);
}

void exit_tablet_mode() {
    const char *home = std::getenv("HOME");
    if (!home) {
        std::cout << "there is no home directory" << '\n';
        return;
    }
    std::filesystem::path script = std::filesystem::path(home) / ".config/eww/close_dashboard.sh";
    execute_script(script);
}
