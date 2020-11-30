#include <iostream>
#include <unistd.h>
#include <string.h>
#include "forecaster.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    Forecaster &forecaster = Forecaster::instance();
    if (!forecaster.parseHostPid(argc, argv)) return 1;

    try {
        bool result = forecaster.handshake();
        if (!result) {
            printErr("Could not establish connection to host");
            return 1;
        }

        forecaster.readDate();
        forecaster.sendPrediction();
    } catch (const char *error) {
        perror(error);
        return 1;
    }
    return 0;
}