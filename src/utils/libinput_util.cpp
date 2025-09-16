#include "utils/libinput_util.h"
#include "gestures.h"
#include <cstdint>
#include <iostream>
#include <libinput.h>
#include <unistd.h>

LibinputContextWrapper libinput_init() {
    struct udev *udev = udev_new();
    struct libinput *li = libinput_udev_create_context(&interface, nullptr, udev);
    libinput_udev_assign_seat(li, "seat0");

    int fd = libinput_get_fd(li);
    return {li, fd};
}

void libinput_poll(LibinputContextWrapper &ctx, SwitchCallback cb) {
    libinput_dispatch(ctx.ctx);

    struct libinput_event *event;
    while ((event = libinput_get_event(ctx.ctx)) != nullptr) {
        if (libinput_event_get_type(event) == LIBINPUT_EVENT_SWITCH_TOGGLE) {
            auto sw_event = libinput_event_get_switch_event(event);
            if (libinput_event_switch_get_switch(sw_event) == LIBINPUT_SWITCH_TABLET_MODE) {
                bool tablet_on = libinput_event_switch_get_switch_state(sw_event) == LIBINPUT_SWITCH_STATE_ON;
                cb(tablet_on);
            }
        } else if (libinput_event_get_type(event) == LIBINPUT_EVENT_TOUCH_DOWN) {
            struct libinput_event_touch *touch_event = libinput_event_get_touch_event(event);
            uint32_t slot = libinput_event_touch_get_slot(touch_event);
            double x = libinput_event_touch_get_x(touch_event);
            double y = libinput_event_touch_get_y(touch_event);
            handle_touch_down(slot, x, y);
        } else if (libinput_event_get_type(event) == LIBINPUT_EVENT_TOUCH_MOTION) {
            struct libinput_event_touch *touch_event = libinput_event_get_touch_event(event);
            uint32_t slot = libinput_event_touch_get_slot(touch_event);
            double x = libinput_event_touch_get_x(touch_event);
            double y = libinput_event_touch_get_y(touch_event);
            handle_touch_motion(slot, x, y);
        } else if (libinput_event_get_type(event) == LIBINPUT_EVENT_TOUCH_FRAME) {
            handle_gesture();
        } else if (libinput_event_get_type(event) == LIBINPUT_EVENT_TOUCH_UP) {
            struct libinput_event_touch *touch_event = libinput_event_get_touch_event(event);
            uint32_t slot = libinput_event_touch_get_slot(touch_event);
            handle_up(slot);
        }
        libinput_event_destroy(event);
    }
}

void libinput_cleanup(LibinputContextWrapper &ctx) { libinput_unref(ctx.ctx); }
