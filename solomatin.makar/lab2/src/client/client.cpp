#include <iostream>
#include <unistd.h>
#include <string.h>
#include "forecaster.h"
#include "print_utils.h"

int main(int argc, char *argv[]) {
    Forecaster &forecaster = Forecaster::instance();
    try {
        forecaster.parse(argc, argv);
        forecaster.handshake();
        forecaster.forecast();
    } catch (const char *error) {
        printErr(error);

        if (errno != 0) perror(error);
        return 1;
    }
    return 0;
}