#pragma once
#include <libinput.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void handle_switch_event(libinput_event *e, int client_socket);
