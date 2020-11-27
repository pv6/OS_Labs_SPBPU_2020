#ifndef LAB1_HOST_H
#define LAB1_HOST_H

#include <semaphore.h>
#include <DTO.h>
#include <ConnectionInfo.h>
#include <IConnection.h>
#include <csignal>

class Host {
public:

    Host(Host &) = delete;

    Host(const Host &) = delete;

    Host &operator=(const Host &) = delete;

    static Host &getInstance();

    void openConnection();

    void start();

    void terminate();

private:
    static const std::string month;
    static const std::string day;
    static const std::string year;
    bool work = true;
    IConnection connection;
    sem_t *semaphore_host{};
    sem_t *semaphore_client{};
    ConnectionInfo connectionInfo;
    int current_number{};

    DTO getDate();

    unsigned int getDataFromUser(int end, const std::string &type);

    static void weatherForDate(DTO &answer);

    static void hanleSignal(int signum, siginfo_t *info, void *ptr);

    Host();
};


#endif //LAB1_HOST_H
