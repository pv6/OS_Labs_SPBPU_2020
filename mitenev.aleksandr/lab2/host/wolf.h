//
// Created by aleksandr on 26.11.2020.
//

#ifndef LAB2_WOLF_H
#define LAB2_WOLF_H

#include <semaphore.h>
#include <syslog.h>
#include <csignal>
#include "../support/clientInfo.h"
#include "../support/message.h"
#include "../conn/conn.h"

class Wolf{
public:
    static Wolf& getWolf();
    bool openConnection();
    void run();
    Wolf(Wolf&) = delete;
    Wolf(const Wolf&) = delete;
    Wolf& operator=(const Wolf&) = delete;
private:
    Conn connection;
    sem_t* semaphore_host;
    sem_t* semaphore_client;
    ClientInfo client_info;
    int wolf_num;
    bool status = true;

    Wolf();
    void updateStatus(Message& answer);
    void terminate();
    void getWolfNum();
    static void signalHandler(int signum, siginfo_t* info, void *ptr);
};
#endif //LAB2_WOLF_H
