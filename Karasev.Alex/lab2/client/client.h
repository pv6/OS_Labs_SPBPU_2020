#ifndef LAB2_CLIENT_H
#define LAB2_CLIENT_H

#include <csignal>
#include <semaphore.h>
#include <map>
#include "../connect/IConnection.h"
#include "../connect/DateAndTemp.h"

class Client {
public:
    Client(Client &) = delete;
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;
    static Client& getInstance(int host_pid, sem_t* sem_host, sem_t* sem_client);
    void run();
    void openConnection();
    ~Client();

    explicit Client(int host_pid, sem_t* sem_host, sem_t* sem_client);

private:
    IConnection connection;
    sem_t *semaphore_host;
    sem_t *semaphore_client;
    int hostPid;
    bool work = true;

    static int getTempByDate(const DateAndTemp& data);
    //static void term_handler(int signum, siginfo_t* info, void* ptr);
};


#endif //LAB2_CLIENT_H