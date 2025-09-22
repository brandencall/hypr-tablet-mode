#include "input_daemon/libinput_util.h"
#include "input_daemon/gestures.h"
#include "input_daemon/tablet.h"

using HandlerFn = std::function<void(libinput_event *, int)>;

std::unordered_map<libinput_event_type, HandlerFn> handlers = {
    {LIBINPUT_EVENT_SWITCH_TOGGLE, [](auto *e, int client_socket) { handle_switch_event(e, client_socket); }},
    {LIBINPUT_EVENT_TOUCH_DOWN, [](auto *e, int client_socket) { handle_touch_down_event(e); }},
    {LIBINPUT_EVENT_TOUCH_MOTION, [](auto *e, int client_socket) { handle_touch_motion_event(e); }},
    {LIBINPUT_EVENT_TOUCH_UP, [](auto *e, int client_socket) { handle_touch_up_event(e, client_socket); }},
    {LIBINPUT_EVENT_TOUCH_FRAME, [](auto *e, int client_socket) { handle_gesture(); }},
};

LibinputContextWrapper libinput_init() {
    struct udev *udev = udev_new();
    struct libinput *li = libinput_udev_create_context(&interface, nullptr, udev);
    libinput_udev_assign_seat(li, "seat0");

    int fd = libinput_get_fd(li);
    return {li, fd};
}

int libinput_poll(LibinputContextWrapper &ctx, int client_socket) {
    libinput_dispatch(ctx.ctx);

    struct libinput_event *event;
    while ((event = libinput_get_event(ctx.ctx)) != nullptr) {
        auto type = libinput_event_get_type(event);
        if (handlers.find(type) != handlers.end()) {
            handlers[type](event, client_socket);
        }
        libinput_event_destroy(event);
    }
    return 1;
}

void libinput_cleanup(LibinputContextWrapper &ctx) { libinput_unref(ctx.ctx); }
