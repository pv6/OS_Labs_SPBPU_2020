#ifndef LAB1_CLIENT_H
#define LAB1_CLIENT_H

#include <csignal>
#include <semaphore.h>
#include <map>
#include <IConnection.h>

class Client {
public:
    Client(Client &) = delete;

    Client(const Client &) = delete;

    Client &operator=(const Client &) = delete;

    static Client &getInstance(int host_pid);

    void start();

    void openConnection();

    void terminate();

private:
    IConnection connection;
    sem_t *semaphore_host;
    sem_t *semaphore_client;
    int host_pid;
    bool work = true;

    explicit Client(int host_pid);

    int getWeather(const DTO &dto);

    static void handleSignal(int signum);
};


#endif //LAB1_CLIENT_H
