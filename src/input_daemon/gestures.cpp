#include "input_daemon/gestures.h"
#include "ipc/socket_server.h"
#include <iostream>

std::map<uint32_t, Finger> active_fingers;
Gesture current_gesture = None;
State current_state = Idle;

void reset() {
    current_gesture = None;
    current_state = Idle;
}

void handle_touch_down_event(libinput_event *e) {
    struct libinput_event_touch *touch_event = libinput_event_get_touch_event(e);
    uint32_t slot = libinput_event_touch_get_slot(touch_event);
    double x = libinput_event_touch_get_x(touch_event);
    double y = libinput_event_touch_get_y(touch_event);
    Finger f;
    f.init_x = f.last_x = x;
    f.init_y = f.last_y = y;
    active_fingers[slot] = f;
    current_state = TouchActive;
}

void handle_touch_motion_event(libinput_event *e) {
    struct libinput_event_touch *touch_event = libinput_event_get_touch_event(e);
    uint32_t slot = libinput_event_touch_get_slot(touch_event);
    double x = libinput_event_touch_get_x(touch_event);
    double y = libinput_event_touch_get_y(touch_event);
    auto &f = active_fingers[slot];
    f.last_x = x;
    f.last_y = y;
    current_state = GestureTracking;
}

void handle_touch_up_event(libinput_event *e, int client_socket) {
    struct libinput_event_touch *touch_event = libinput_event_get_touch_event(e);
    uint32_t slot = libinput_event_touch_get_slot(touch_event);
    active_fingers.erase(slot);
    if (active_fingers.size() == 0) {
        if (current_gesture == CloseWindow) {
            std::cout << "CloseWindow gesture" << '\n';
            json msg = {{"type", "hypr"}, {"event", "close_window"}};
            if (!write_client(client_socket, msg.dump())) {
                std::cout << "failed to write to client" << '\n';
            }
        } else if (current_gesture == WorkspaceRight) {
            std::cout << "WorkspaceRight gesture" << '\n';
            json msg = {{"type", "hypr"}, {"event", "workspace_right"}};
            if (!write_client(client_socket, msg.dump())) {
                std::cout << "failed to write to client" << '\n';
            }
        } else if (current_gesture == WorkspaceLeft) {
            std::cout << "WorkspaceLeft gesture" << '\n';
            json msg = {{"type", "hypr"}, {"event", "workspace_left"}};
            if (!write_client(client_socket, msg.dump())) {
                std::cout << "failed to write to client" << '\n';
            }
        }
        reset();
    }
}

void handle_gesture() {
    if (current_state != GestureTracking && active_fingers.size() < 2)
        return;

    for (auto &finger : active_fingers) {
        Vector v = calculate_motion_vector(finger.second);
        finger.second.vector = v;
    }
    close_window_gesture();
    workspace_right_gesture();
    workspace_left_gesture();
}

Vector calculate_motion_vector(const Finger &finger) {
    Vector vector;
    vector.x = finger.last_x - finger.init_x;
    vector.y = finger.last_y - finger.init_y;
    vector.magnitue = std::sqrt(vector.x * vector.x + vector.y * vector.y);
    return vector;
}

void close_window_gesture() {
    if (active_fingers.size() != 2)
        return;

    Gesture finger_gesture = None;
    for (const auto &finger : active_fingers) {
        if (finger.second.vector.x > -50 && finger.second.vector.x < 50 && finger.second.vector.y < 0 &&
            finger.second.vector.magnitue > 10) {
            finger_gesture = CloseWindow;
        } else if (current_gesture == CloseWindow) {
            current_gesture = None;
            return;
        } else {
            return;
        }
    }
    current_gesture = finger_gesture;
}

void workspace_right_gesture() {
    if (active_fingers.size() != 2)
        return;

    Gesture finger_gesture = None;
    for (const auto &finger : active_fingers) {
        if (finger.second.vector.y > -50 && finger.second.vector.y < 50 && finger.second.vector.x < 0 &&
            finger.second.vector.magnitue > 10) {
            finger_gesture = WorkspaceRight;
        } else if (current_gesture == WorkspaceRight) {
            current_gesture = None;
            return;
        } else {
            return;
        }
    }
    current_gesture = finger_gesture;
}

void workspace_left_gesture() {
    if (active_fingers.size() != 2)
        return;

    Gesture finger_gesture = None;
    for (const auto &finger : active_fingers) {
        if (finger.second.vector.y > -50 && finger.second.vector.y < 50 && finger.second.vector.x > 0 &&
            finger.second.vector.magnitue > 10) {
            finger_gesture = WorkspaceLeft;
        } else if (current_gesture == WorkspaceLeft) {
            current_gesture = None;
            return;
        } else {
            return;
        }
    }
    current_gesture = finger_gesture;
}
