#pragma once

#include <signal.h>
#include "date.h"

class Forecaster {
    bool signalHandled = false; 
    int prediction;   // current prediciton on date
    int hostPid = -1; // pid of host
    Date date;        // date to forecast

    

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
    void predict();
    void sendPrediction();

    ~Forecaster();
};