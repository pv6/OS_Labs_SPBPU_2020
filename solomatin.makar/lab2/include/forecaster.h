#pragma once

#include <signal.h>
#include "date.h"

class Forecaster {
    const double timeout = 10;
    const double sleepTime = 0.5;
    bool signalHandled;

    int hostPid;
    Date date;

    Forecaster();
    Forecaster(const Forecaster&) = delete;
    Forecaster &operator=(const Forecaster&) = delete;

    static void handleSignal(int signum, siginfo_t* info, void* ptr);
public:
    static Forecaster &instance() {
        static Forecaster instance;
        return instance;
    }
    bool handshake();

    bool parseHostPid(int argc, char *argv[]);
    int getHostPid() { return hostPid; }

    int predict();

    Date getDate() { return date; }

    int forecast();
    ~Forecaster();
};