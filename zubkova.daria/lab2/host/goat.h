#ifndef LAB2_GOAT_H
#define LAB2_GOAT_H
#include "message.h"
#include "../conn/conn.h"
#include <unistd.h>
#include <semaphore.h>
#include <iostream>

class Goat {
public:
    static Goat* GetInstance(int id);
    void Start();
    Goat(int id);
    void Set(conn* connection, sem_t* semHost, sem_t* semClient);
    ~Goat();
    Status GetStatus();
    conn* GetConnection();
    sem_t* GetSemHost();
    sem_t* GetSemClient();
    pid_t GetPid();
    void SetPid(pid_t p);
    void Terminate();
private:
    pid_t pid;
    Status status;
    conn* connection;
    sem_t* semClient;
    sem_t* semHost;
    int myId;
    bool st = true;
    static int GenerateRandomNumber(int max);
    static void SignalHandler(int signum);
};


#endif //LAB2_GOAT_H
