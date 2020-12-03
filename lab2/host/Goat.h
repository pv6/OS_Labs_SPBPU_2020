//
// Created by yudzhinnsk on 27.11.2020.
//

#ifndef LASTTRY_GOAT_H
#define LASTTRY_GOAT_H

#include "../conn/Conn.h"
#include <semaphore.h>

class Goat {
public:
    static Goat* GetInstance(int host_pid);
    void StartRand();
    bool CreateConnection();
    void set_conn(Conn connection);
private:
    Conn _connection;
    sem_t* semaphore_host;
    sem_t* semaphore_client;
    std::string sem_client_name;
    std::string sem_host_name;

    int host_pid;

    Goat(int host_pid);
    Goat(Goat &weather);
    Goat &operator=(Goat &weather);
    void Terminate(int signum);
    static void SignalHandler(int signum);
};
#endif //LASTTRY_GOAT_H
