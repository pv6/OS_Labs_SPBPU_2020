//
// Created by aleksandr on 26.11.2020.
//

#ifndef LAB2_GOAT_H
#define LAB2_GOAT_H

#include <semaphore.h>
#include <syslog.h>
#include "../conn/conn.h"

class Goat{
public:
    void run();

    bool openConnection();

    static Goat& getGoat(int host_pid);

    void terminate();

    Goat(Goat&) = delete;
    Goat(const Goat&) = delete;
    Goat &operator=(const Goat&) = delete;

private:

    int getNum(int max);

    Goat(int host_pid);

    static void signalHandler(int signum);

    int host_pid;
    bool status = true;
    Conn connection;
    sem_t* semaphore_client = nullptr;
    sem_t* semaphore_host = nullptr;
};


#endif //LAB2_GOAT_H
