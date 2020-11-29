#include <iostream>
#include <unistd.h>
#include <string.h>
#include "forecaster.h"

int main(int argc, char *argv[]) {
    Forecaster &forecaster = Forecaster::instance();
    if (!forecaster.parseHostPid(argc, argv)) return 1;

    bool result = forecaster.handshake();
    if (!result) {
        std::cout << "Could not establish connection to host with pid " << forecaster.getHostPid() << std::endl;
        return 1;
    }

    forecaster.requestDate(); 
    forecaster.predict();
    forecaster.sendPrediction();
    return 0;
}