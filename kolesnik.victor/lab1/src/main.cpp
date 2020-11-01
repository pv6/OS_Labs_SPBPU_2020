#include "../include/DaemonManager.h"


int main(int argc, char **argv) {
    std::string pid_path;
    std::string config_path = "config.txt";

    if (argc >= 2) {
        pid_path = argv[1];
    } else {
        pid_path = "/var/run/lab1.pid";
    }

    if (DaemonManager::create_DaemonManager(pid_path, config_path)) {
        DaemonManager::run();
    }

    return 0;
}