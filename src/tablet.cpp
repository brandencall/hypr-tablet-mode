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
