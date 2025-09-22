#include "hypr_control/tablet.h"
#include "hypr_control/hypr_util.h"
#include <iostream>

void handle_tablet_event(const std::string &event) {
    if (event == "tablet_on") {
        enter_tablet_mode();
    } else if (event == "tablet_off") {
        exit_tablet_mode();
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

void execute_script(std::filesystem::path script) {
    if (fork() == 0) {
        execl(script.c_str(), script.c_str(), (char *)nullptr);
        _exit(127);
    }
}
