#include <syslog.h>
#include <cstring>
#include "client.h"
#include "../connect/Connection_info.h"
#include "../connect/DateAndTemp.h"

Client& Client::getInstance(int host_pid, sem_t* sem_host, sem_t* sem_client){
    static Client client(host_pid, sem_host, sem_client);
    return client;
}

Client::Client(int host_pid, sem_t* sem_host, sem_t* sem_client) :
        semaphore_host{sem_host}, semaphore_client{sem_client}, hostPid{host_pid} {
    /*static struct sigaction _sigact;
    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = Client::term_handler;
    _sigact.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &_sigact, nullptr);*/
};

void Client::openConnection() {
    syslog(LOG_INFO, "Client open connect");
    connection.openConnection(hostPid, false);
}

void Client::run() {
    timespec ts{};
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout;
    DateAndTemp data;
    syslog(LOG_INFO, "wait host connect");
    //pause();
    if (sem_timedwait(semaphore_client, &ts) == -1) {
        throw std::runtime_error("timeout " + std::string(strerror(errno)));
    }
    syslog(LOG_INFO, "host connected");
    work = true;
    while (work) {
        syslog(LOG_INFO, "wait host data");
        sem_wait(semaphore_client);
        connection.readConnection(&data);
        int temp = getTempByDate(data);
        data.setTemp(temp);
        syslog(LOG_INFO, "Income date: %i.%i.%i", data.getDay(), data.getMonth(), data.getYear());
        syslog(LOG_INFO, "calculated temp: %i", temp);
        connection.writeConnection(&data);
        sem_post(semaphore_host);
    }
}

int Client::getTempByDate(const DateAndTemp &data) {
    long date = data.getYear() * 31 * 12 + data.getMonth() * 31 + data.getDay();
    srand(date);
    return rand() / float (RAND_MAX) * (DateAndTemp::maxTemp - DateAndTemp::minTemp) + DateAndTemp::minTemp;
}

Client::~Client() {
    connection.closeConnection();
}

