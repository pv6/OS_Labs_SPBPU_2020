#ifndef LAB2_GOAT_H
#define LAB2_GOAT_H

#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <csignal>
#include <cstring>
#include <random>

#include "../interfaces/Conn.h"
#include "../interfaces/Message.h"


class Goat{
public:
    static Goat &getInstance();

    void run(int argc, char* argv[]);
private:
    Goat();
    Goat(Goat const &);
    Goat &operator=(Goat const &);

    static const int LIMIT_ALIVE = 100;
    static const int LIMIT_DEAD = 50;

    void terminate(int sig);

    bool openConnection();

    void startWork();

    static void signalHandler(int signum, siginfo_t *info, void* ptr);

    static int getRand(Status status);


    int hostPid_;
    int id_;
    Conn connection_;
    sem_t* semaphoreHost_;
    sem_t* semaphoreClient_;
};


#endif //LAB2_GOAT_H
