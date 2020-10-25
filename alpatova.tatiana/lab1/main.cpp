#include <string>
#include <iostream>
#include "daemon.h"

int main(int argc, char **argv) {
    std::string config_file_path;
    if (argc != 2) {
        std:: cout << "ERROR: nums of args are not valid" << std::endl;
        return EXIT_FAILURE;
    }
    std::string conf_file= argv[1];
    daemon::set_config(conf_file);

    if (daemon ::create() == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    daemon ::run();
    return EXIT_SUCCESS;
}