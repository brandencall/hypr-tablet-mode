#pragma once
#include <cstdlib>
#include <filesystem>
#include <iostream>

void handle_tablet_event(const std::string &event);
void enter_tablet_mode();
void exit_tablet_mode();
void execute_script(std::filesystem::path script);
void kill_waybar();
void launch_waybar();
void execute_script(std::filesystem::path path, std::string cmd, std::string process);
void execute_script(std::filesystem::path path, std::string cmd);
