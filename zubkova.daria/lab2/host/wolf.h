#ifndef LAB2_WOLF_H
#define LAB2_WOLF_H

#include "../conn/conn.h"
#include "goat.h"
#include <semaphore.h>
#include <map>
#include <list>

typedef struct str{
    int pid;
    conn* con;
    sem_t* semHost;
    sem_t* semClient;
    int id;
}str;

class Wolf {
public:
    static Wolf* GetInstance();
    void Start();
    ~Wolf();
private:
    static void* ReadGoats(void* param);
    static void* WriteGoats(void* param);
    static void KillClient(str* client);
    static void Terminate(int signum);
    static void SignalHandler(int signum, siginfo_t* info, void* ptr);
    static std::list<str*> structClients;
    static std::map<int, Message> clientsMessages;
    static int numGoats;
    static int numWolf;
    Wolf();
    Wolf(Wolf& other);
    Wolf& operator=(Wolf& other);
    static bool CreateGoats();
    void Process();
    int st = true;
    static void Threads(void* (*function) (void*));
};


#endif //LAB2_WOLF_H
