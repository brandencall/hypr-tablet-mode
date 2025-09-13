#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

using json = nlohmann::json;

int connect_to_hypr();
int execute_hypr_cmd(std::string cmd);
