#pragma once

#include <cmath>
#include <cstdint>
#include <iostream>
#include <libinput.h>
#include <map>

struct Finger {
  double init_x, init_y;
  double last_x, last_y;
};

struct Vector {
  double x, y;
  double magnitue;
};

void handle_touch_down_event(libinput_event *e);
void handle_touch_motion_event(libinput_event *e);
void handle_touch_up_event(libinput_event *e);
void handle_gesture();

Vector calculate_motion_vector(const Finger &finger);
