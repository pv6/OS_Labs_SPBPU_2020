#pragma once

#include <signal.h>
#include "date.h"
#include "connection.h"

class Forecaster {
    int hostPid;
    int id;
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

    void parse(int argc, char *argv[]);
    void handshake();
    void forecast();

    ~Forecaster();
};