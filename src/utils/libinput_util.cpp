#include "utils/libinput_util.h"
#include "gestures.h"
#include <functional>
#include <libinput.h>
#include <unistd.h>
#include <unordered_map>

using HandlerFn = std::function<void(libinput_event *)>;

void handle_switch_event(libinput_event *e) {
    auto sw_event = libinput_event_get_switch_event(e);
    if (libinput_event_switch_get_switch(sw_event) == LIBINPUT_SWITCH_TABLET_MODE) {
        bool tablet_on = libinput_event_switch_get_switch_state(sw_event) == LIBINPUT_SWITCH_STATE_ON;
    }
}

std::unordered_map<libinput_event_type, HandlerFn> handlers = {
    {LIBINPUT_EVENT_SWITCH_TOGGLE, [](auto *e) { handle_switch_event(e); }},
    {LIBINPUT_EVENT_TOUCH_DOWN, [](auto *e) { handle_touch_down_event(e); }},
    {LIBINPUT_EVENT_TOUCH_MOTION, [](auto *e) { handle_touch_motion_event(e); }},
    {LIBINPUT_EVENT_TOUCH_UP, [](auto *e) { handle_touch_up_event(e); }},
    {LIBINPUT_EVENT_TOUCH_FRAME, [](auto *_) { handle_gesture(); }},
};

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
        // TODO: need a better way of handling Switch event so that it can be handled by the function table
        auto type = libinput_event_get_type(event);
        if (type == LIBINPUT_EVENT_SWITCH_TOGGLE) {
            auto sw_event = libinput_event_get_switch_event(event);
            if (libinput_event_switch_get_switch(sw_event) == LIBINPUT_SWITCH_TABLET_MODE) {
                bool tablet_on = libinput_event_switch_get_switch_state(sw_event) == LIBINPUT_SWITCH_STATE_ON;
                cb(tablet_on);
            }
        } else if (handlers.find(type) != handlers.end()) {
            handlers[type](event);
        }
        libinput_event_destroy(event);
    }
}

void libinput_cleanup(LibinputContextWrapper &ctx) { libinput_unref(ctx.ctx); }
