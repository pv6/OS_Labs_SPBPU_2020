#ifndef LAB2_WOLF_H
#define LAB2_WOLF_H

#include "../conn/conn.h"
#include "goat.h"
#include <semaphore.h>
#include <map>
#include <list>

class Wolf {
public:
    static Wolf* GetInstance();
    void Start();
    ~Wolf();
private:
    static void* ReadGoats(void* param);
    static void* WriteGoats(void* param);
    static void KillClient(Goat* client);
    static void Terminate(int signum);
    static void SignalHandler(int signum, siginfo_t* info, void* ptr);
    static std::list<Goat*> clients;
    static std::map<Goat*, Message> clientsMessages;
    static int numGoats;
    static int numWolf;
    Wolf();
    static void CreateGoats();
    void Process();
    static void Threads(void* (*function) (void*));
};


#endif //LAB2_WOLF_H
