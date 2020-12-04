#ifndef HOST_H
#define HOST_H

#include <semaphore.h>
#include <csignal>
#include "../connect/DateAndTemp.h"
#include "../connect/Connection_info.h"
#include "../connect/IConnection.h"

class Host {
public:
    Host(Host &) = delete;
    Host(const Host &) = delete;
    Host &operator=(const Host &) = delete;

    static Host& getInstance(int pid);
    void openConnection(sem_t* sem_host, sem_t *sem_client);
    void run();
    void terminate();

private:
    bool work = true;
    IConnection connection;
    ConnectionInfo connInfo;
    sem_t *semaphore_host{};
    sem_t *semaphore_client{};
    
    DateAndTemp message;

    void printDateAndTemp();

    Host(int pid);

    bool inputDate();

    static bool TurnOff(unsigned day, unsigned month, unsigned year);
};


#endif //HOST_H