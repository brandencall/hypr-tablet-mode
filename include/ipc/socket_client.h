#pragma once

#include "nlohmann/json.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

using SocketEventCallback = std::function<void(const std::string &)>;

int connect_to_server();
void handle_session(int socket, SocketEventCallback cb);
