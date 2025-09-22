#include "hypr_control/hypr_util.h"
#include "hypr_control/tablet.h"
#include "ipc/socket_client.h"
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

using json = nlohmann::json;

int main(void) {
    int socket_fd = connect_to_server();
    handle_session(socket_fd, [](const std::string &msg) {
        json data = json::parse(msg);
        std::string type = data["type"];
        if (type == "hypr") {
            std::string event = data["event"];
            std::cout << "type: " << type << " event: " << event << '\n';
            handle_hypr_event(event);
        } else if (type == "tablet") {
            std::string event = data["event"];
            std::cout << "type: " << type << " event: " << event << '\n';
            handle_tablet_event(event);
        }
    });
    return 0;
}
