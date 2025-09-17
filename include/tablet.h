#pragma once
#include "utils/hypr_util.h"
#include <libinput.h>

void handle_switch_event(libinput_event *e);
void enter_tablet_mode();
void exit_tablet_mode();
