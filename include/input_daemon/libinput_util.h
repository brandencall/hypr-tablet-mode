#pragma once
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <libinput.h>
#include <libudev.h>
#include <poll.h>
#include <sys/poll.h>
#include <unistd.h>
#include <unordered_map>


static int open_restricted(const char *path, int flags, void *user_data) {
  int fd = open(path, flags);
  if (fd < 0)
    perror("Failed to open device");
  return fd;
}

static void close_restricted(int fd, void *user_data) { close(fd); }

static const struct libinput_interface interface = {
    .open_restricted = open_restricted,
    .close_restricted = close_restricted,
};

struct LibinputContextWrapper {
  struct libinput *ctx;
  int fd;
};

LibinputContextWrapper libinput_init();

int libinput_poll(LibinputContextWrapper &ctx, int client_socket);

void libinput_cleanup(LibinputContextWrapper &ctx);
