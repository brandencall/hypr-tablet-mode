#include "gestures.h"
#include <cmath>
#include <iostream>

std::map<uint32_t, Finger> active_fingers;

void handle_touch_down(uint32_t slot, double x, double y) {
    Finger f;
    f.init_x = f.last_x = x;
    f.init_y = f.last_y = y;
    active_fingers[slot] = f;
}

void handle_touch_motion(uint32_t slot, double x, double y) {
    auto &f = active_fingers[slot];
    f.last_x = x;
    f.last_y = y;
}

void handle_up(uint32_t slot) {
    std::cout << "Erasing Finger: " << slot << '\n';
    active_fingers.erase(slot);
}

void handle_gesture() {
    for (const auto &finger : active_fingers) {
        Vector v = calculate_motion_vector(finger.second);
        if (active_fingers.size() == 2 && v.y < 0 && v.magnitue > 30) {
            std::cout << "Close gesture detected" << '\n';
        }
        // std::cout << "Finger: " << finger.first << " vector.x: " << v.x << " vector.y: " << v.y
        //           << " vector.mag: " << v.magnitue << '\n';
    }
}

Vector calculate_motion_vector(const Finger &finger) {
    Vector vector;
    vector.x = finger.last_x - finger.init_x;
    vector.y = finger.last_y - finger.init_y;
    vector.magnitue = std::sqrt(vector.x * vector.x + vector.y * vector.y);
    return vector;
}
