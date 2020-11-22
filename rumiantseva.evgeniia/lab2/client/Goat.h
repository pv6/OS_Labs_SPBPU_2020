//
// Created by Evgenia on 08.11.2020.
//

#ifndef LAB2_GOAT_H
#define LAB2_GOAT_H

#include "../conn/conn.h"



class Goat{
public:
    void Start();

    bool SetupConnection();

    static Goat& GetGoatInst();

    void SetHostPid(int pid);

    Goat(Goat &) = delete;
    Goat &operator=(const Goat &) = delete;

private:
    int host_pid;

    void Terminate(int signum);

    Goat();

    static void SignalHandler(int signum, siginfo_t *info, void *ptr);

    int client_id;
    bool is_running = true;
    Conn connection;
    sem_t* semaphore;
    sem_t* semaphore_host;
};


#endif //LAB2_GOAT_H
