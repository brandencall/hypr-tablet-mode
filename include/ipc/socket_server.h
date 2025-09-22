#pragma once

#include <fcntl.h>
#include <grp.h>
#include <iostream>
#include <libinput.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <poll.h>
#include <pwd.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <systemd/sd-bus.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

using json = nlohmann::json;

int create_server_socket();
void client_session(int client_socket);
bool write_client(int client_socket, const std::string &msg);
