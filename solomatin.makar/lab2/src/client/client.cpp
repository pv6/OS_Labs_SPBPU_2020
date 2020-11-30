#include <iostream>
#include <unistd.h>
#include <string.h>
#include "forecaster.h"
#include "print_utils.h"

int main(int argc, char *argv[]) {
    Forecaster &forecaster = Forecaster::instance();
    if (!forecaster.parseHostPid(argc, argv)) return 1;

    try {
        bool result = forecaster.handshake();
        if (!result) {
            printErr("Could not establish connection to host");
            return 1;
        }
    } catch (const char *error) {
        perror(error);
        return 1;
    }
    return 0;
}