#pragma once

#include <cmath>
#include <cstdint>
#include <iostream>
#include <libinput.h>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum Gesture { None, CloseWindow, WorkspaceRight, WorkspaceLeft };

enum State { Idle, TouchActive, GestureTracking };

struct Vector {
  double x, y;
  double magnitue;
};

struct Finger {
  double init_x, init_y;
  double last_x, last_y;
  Vector vector;
};

void reset();
void handle_touch_down_event(libinput_event *e);
void handle_touch_motion_event(libinput_event *e);
void handle_touch_up_event(libinput_event *e, int client_socket);
void handle_gesture();
Vector calculate_motion_vector(const Finger &finger);
void close_window_gesture();
void workspace_right_gesture();
void workspace_left_gesture();
