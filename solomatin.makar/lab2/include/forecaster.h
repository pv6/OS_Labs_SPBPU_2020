#pragma once

#include <signal.h>
#include "date.h"
#include "connection.h"

class Forecaster {
    int prediction;   // current prediciton on date
    int hostPid = -1; // pid of host
    Date date;        // date to forecast
    Connection *connection = nullptr;

    Forecaster();
    Forecaster(const Forecaster&) = delete;
    Forecaster &operator=(const Forecaster&) = delete;

    static void handleSignal(int signum, siginfo_t* info, void* ptr);
public:
    static Forecaster &instance() {
        static Forecaster instance;
        return instance;
    }

    bool parseHostPid(int argc, char *argv[]);

    bool handshake();
    void readDate();
    void sendPrediction();

    ~Forecaster();
};