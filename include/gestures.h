#pragma once

#include <cstdint>
#include <map>
struct Finger {
  double init_x, init_y;
  double last_x, last_y;
};

struct Vector {
  double x, y;
  double magnitue;
};

void handle_touch_down(uint32_t slot, double x, double y);
void handle_touch_motion(uint32_t slot, double x, double y);
void handle_up(uint32_t slot);
void handle_gesture();
Vector calculate_motion_vector(const Finger &finger);
