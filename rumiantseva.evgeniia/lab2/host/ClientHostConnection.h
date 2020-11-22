//
// Created by Evgenia on 08.11.2020.
//

#ifndef LAB2_CLIENTHOSTCONNECTION_H
#define LAB2_CLIENTHOSTCONNECTION_H


#include "../conn/conn.h"


class ClientHostConnection {

public:
    bool HasSignalledBack();

    int GetClientPid();

    Conn& GetConnection();

    sem_t& GetSemaphore();
    sem_t& GetHostSemaphore();

    bool SetupConnection(int id); // send initial signal to client

    void OnSignalBack(int pid); // on client response

    void Delete();

    void SetAlive();
    void IncrDaysDead();
    int GetDaysDead();

private:
    bool signal_back = false;
    int client_id;
    int client_pid;
    Conn connection;
    sem_t *semaphore;
    sem_t *semaphore_host;
    int days_dead = 0;
};



#endif //LAB2_CLIENTHOSTCONNECTION_H
