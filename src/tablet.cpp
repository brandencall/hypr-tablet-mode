#include "tablet.h"
#include <iostream>

void enter_tablet_mode() { int hypr_write = execute_hypr_cmd("dispatch workspace empty"); }

void exit_tablet_mode() { std::cout << "exit_tablet_mode()" << '\n'; }
