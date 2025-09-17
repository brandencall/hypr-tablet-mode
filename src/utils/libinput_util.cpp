#include "utils/libinput_util.h"
#include "gestures.h"
#include "tablet.h"

using HandlerFn = std::function<void(libinput_event *)>;

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

void libinput_poll(LibinputContextWrapper &ctx) {
    libinput_dispatch(ctx.ctx);

    struct libinput_event *event;
    while ((event = libinput_get_event(ctx.ctx)) != nullptr) {
        auto type = libinput_event_get_type(event);
        if (handlers.find(type) != handlers.end()) {
            handlers[type](event);
        }
        libinput_event_destroy(event);
    }
}

void libinput_cleanup(LibinputContextWrapper &ctx) { libinput_unref(ctx.ctx); }
