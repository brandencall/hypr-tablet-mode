#pragma once
#include <cstdlib>
#include <filesystem>
#include <iostream>

void handle_tablet_event(const std::string &event);
void enter_tablet_mode();
void exit_tablet_mode();
void execute_script(std::filesystem::path script);
