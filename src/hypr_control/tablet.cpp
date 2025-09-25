#include "hypr_control/tablet.h"
#include "hypr_control/hypr_util.h"
#include <iostream>
#include <string>

bool tablet_on = false;

// TODO: Add the upside down view
void handle_tablet_event(const std::string &event) {
    if (event == "tablet_on") {
        enter_tablet_mode();
        tablet_on = true;
    } else if (event == "tablet_off") {
        exit_tablet_mode();
        tablet_on = false;
    } else if (event == "left-up" && tablet_on) {
        std::cout << "handle left up" << '\n';
        handle_left_up();
    } else if (event == "right-up" && tablet_on) {
        std::cout << "handle right up" << '\n';
        handle_right_up();
    } else if (event == "normal" && tablet_on) {
        std::cout << "handle normal" << '\n';
        handle_normal_view();
    }
}

void enter_tablet_mode() {
    int hypr_write = execute_hypr_cmd("dispatch workspace empty");
    const char *home = std::getenv("HOME");
    if (!home) {
        std::cout << "there is no home directory" << '\n';
        return;
    }

    std::filesystem::path dashboard = std::filesystem::path(home) / ".config/eww/launch_dashboard.sh";
    execute_script(dashboard);
    kill_waybar();
}

// Should probably run the handle_normal_view() script
void exit_tablet_mode() {
    const char *home = std::getenv("HOME");
    if (!home) {
        std::cout << "there is no home directory" << '\n';
        return;
    }
    std::filesystem::path close_dashboard = std::filesystem::path(home) / ".config/eww/close_dashboard.sh";
    execute_script(close_dashboard);
    launch_waybar();
}

void handle_left_up() {
    const char *home = std::getenv("HOME");
    if (!home) {
        std::cout << "there is no home directory" << '\n';
        return;
    }
    std::filesystem::path script =
        std::filesystem::path(home) / "projects/hypr-tablet-mode/scripts/rotate_monitor_left.sh";
    execute_script(script);
}

void handle_right_up() {
    const char *home = std::getenv("HOME");
    if (!home) {
        std::cout << "there is no home directory" << '\n';
        return;
    }
    std::filesystem::path script =
        std::filesystem::path(home) / "projects/hypr-tablet-mode/scripts/rotate_monitor_right.sh";
    execute_script(script);
}

void handle_normal_view() {
    const char *home = std::getenv("HOME");
    if (!home) {
        std::cout << "there is no home directory" << '\n';
        return;
    }
    std::filesystem::path script =
        std::filesystem::path(home) / "projects/hypr-tablet-mode/scripts/rotate_monitor_normal.sh";
    execute_script(script);
}

void execute_script(std::filesystem::path script) {
    if (fork() == 0) {
        execl(script.c_str(), script.c_str(), (char *)nullptr);
        _exit(127);
    }
}

void kill_waybar() {
    std::filesystem::path kill_script = "/usr/bin/killall";
    std::string kill_cmd = "killall";
    std::string waybar = "waybar";
    if (fork() == 0) {
        execl(kill_script.c_str(), kill_cmd.c_str(), waybar.c_str(), (char *)nullptr);
        _exit(127);
    }
}

void launch_waybar() {
    std::filesystem::path waybar_script = "/usr/bin/waybar";
    std::string waybar = "waybar";
    if (fork() == 0) {
        execl(waybar_script.c_str(), waybar.c_str(), (char *)nullptr);
        _exit(127);
    }
}
