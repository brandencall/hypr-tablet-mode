#pragma once
#include <cstdlib>
#include <filesystem>
#include <iostream>

void handle_tablet_event(const std::string &event);
void enter_tablet_mode();
void exit_tablet_mode();
void handle_left_up();
void handle_right_up();
void execute_script(std::filesystem::path script);
void kill_waybar();
void launch_waybar();
