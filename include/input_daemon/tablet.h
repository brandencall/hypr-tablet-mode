#pragma once
#include <libinput.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool handle_switch_event(libinput_event *e, int client_socket);
void enter_tablet_mode();
void exit_tablet_mode();
