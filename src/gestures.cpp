#include "gestures.h"

std::map<uint32_t, Finger> active_fingers;

void handle_touch_down_event(libinput_event *e) {
    struct libinput_event_touch *touch_event = libinput_event_get_touch_event(e);
    uint32_t slot = libinput_event_touch_get_slot(touch_event);
    double x = libinput_event_touch_get_x(touch_event);
    double y = libinput_event_touch_get_y(touch_event);
    Finger f;
    f.init_x = f.last_x = x;
    f.init_y = f.last_y = y;
    active_fingers[slot] = f;
}

void handle_touch_motion_event(libinput_event *e) {
    struct libinput_event_touch *touch_event = libinput_event_get_touch_event(e);
    uint32_t slot = libinput_event_touch_get_slot(touch_event);
    double x = libinput_event_touch_get_x(touch_event);
    double y = libinput_event_touch_get_y(touch_event);
    auto &f = active_fingers[slot];
    f.last_x = x;
    f.last_y = y;
}

void handle_touch_up_event(libinput_event *e) {
    struct libinput_event_touch *touch_event = libinput_event_get_touch_event(e);
    uint32_t slot = libinput_event_touch_get_slot(touch_event);
    std::cout << "Erasing Finger: " << slot << '\n';
    active_fingers.erase(slot);
}

void handle_gesture() {
    for (const auto &finger : active_fingers) {
        Vector v = calculate_motion_vector(finger.second);
        if (active_fingers.size() == 2 && v.y < 0 && v.magnitue > 30) {
            std::cout << "Close gesture detected" << '\n';
        }
    }
}

Vector calculate_motion_vector(const Finger &finger) {
    Vector vector;
    vector.x = finger.last_x - finger.init_x;
    vector.y = finger.last_y - finger.init_y;
    vector.magnitue = std::sqrt(vector.x * vector.x + vector.y * vector.y);
    return vector;
}
