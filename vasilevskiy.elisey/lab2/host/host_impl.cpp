#include "host.h"
#include <ConnectionConst.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <syslog.h>

const std::string Host::month = "month";
const std::string Host::day = "day";
const std::string Host::year = "year";

Host &Host::getInstance() {
    static Host instance;
    return instance;
}

Host::Host() : connectionInfo(0) {
    struct sigaction structSigaction{};
    memset(&structSigaction, 0, sizeof(structSigaction));
    structSigaction.sa_sigaction = hanleSignal;
    structSigaction.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &structSigaction, nullptr);
    sigaction(SIGUSR1, &structSigaction, nullptr);
    sigaction(SIGUSR2, &structSigaction, nullptr);
    sigaction(SIGINT, &structSigaction, nullptr);
}

void Host::openConnection() {
    syslog(LOG_INFO, "open connection");
    connection.openConnection(getpid(), true);
    semaphore_host = sem_open(ConnectionConst::SEM_HOST_NAME.c_str(), O_CREAT, 0666, 0);
    semaphore_client = sem_open(ConnectionConst::SEM_CLIENT_NAME.c_str(), O_CREAT, 0666, 0);
    if (semaphore_host == SEM_FAILED || semaphore_client == SEM_FAILED) {
        throw std::runtime_error("sem_open failed with error " + std::string(strerror(errno)));
    }
}

void Host::start() {
    timespec ts{};
    DTO dto;
    syslog(LOG_INFO, "wait client connect");
    pause();
    syslog(LOG_INFO, "client attached");
    work = true;
    sem_post(semaphore_client);
    while (work) {
        if (!connectionInfo.isAttached()) {
            syslog(LOG_INFO, "wait client connect");
            while (!connectionInfo.isAttached()) {
                pause();
            }
            if (!work) {
                kill(connectionInfo.getPid(), SIGTERM);
                connectionInfo = ConnectionInfo(0);
                return;
            }
            syslog(LOG_INFO, "client attached");
            sem_post(semaphore_client);
        } else {
            dto = getDate();
            if (connectionInfo.isAttached()){
                connection.writeConnection(&dto);
            }else {
                continue;
            }
            syslog(LOG_INFO, "call client");
            sem_post(semaphore_client);
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += ConnectionConst::TIMEOUT;
            syslog(LOG_INFO, "wait client data");
            if (sem_timedwait(semaphore_host, &ts) == -1) {
                syslog(LOG_INFO, "client timeout");
                if (errno == EINTR) {
                    continue;
                }
                syslog(LOG_INFO, "try turn off client");
                kill(connectionInfo.getPid(), SIGTERM);
                connectionInfo = ConnectionInfo(0);
                continue;
            }
            syslog(LOG_INFO, "do work");
            connection.readConnection(&dto);
            weatherForDate(dto);
        }
    }
}

DTO Host::getDate() {
    bool result = false;
    unsigned int day;
    unsigned int month;
    unsigned int year;
    while (!result && work) {
        tm date{};
        day = getDataFromUser(31, Host::day);
        month = getDataFromUser(12, Host::month);
        year = getDataFromUser(9999, Host::year);
        date.tm_year = year - 1900;
        date.tm_mon = month - 1;
        date.tm_mday = day;
        date.tm_isdst = -1;
        tm tCheck = date;
        mktime(&tCheck);
        if ((date.tm_year != tCheck.tm_year || date.tm_mon != tCheck.tm_mon || date.tm_mday != tCheck.tm_mday) && work)
        {
            result = false;
            std::cout << "please enter correct date" << std::endl;
        }else{
            result = true;
        }
    }
    return DTO(day, month, year);
}

void Host::weatherForDate(DTO &answer) {
    std::cout << answer.getDay() << "." << answer.getMonth() << "." << answer.getYear() << " : " << answer.getTemp()
              << std::endl;
}

void Host::terminate() {
    semaphore_client = SEM_FAILED;
    semaphore_host = SEM_FAILED;
    if (sem_unlink(ConnectionConst::SEM_HOST_NAME.c_str()) == -1 ||
        sem_unlink(ConnectionConst::SEM_CLIENT_NAME.c_str()) == -1) {
        syslog(LOG_ERR, "sem_unlink error : %s", strerror(errno));
    }
    connection.closeConnection();
}

void Host::hanleSignal(int signum, siginfo_t *info, void *ptr) {
    static Host &instance = getInstance();
    switch (signum) {
        case SIGUSR1: {
            if (instance.connectionInfo.isAttached()) {
                syslog(LOG_INFO, "host can process one client");
            } else {
                syslog(LOG_INFO, "Attaching client with pid = %i" ,info->si_pid);
                instance.connectionInfo = ConnectionInfo(info->si_pid);
            }
            break;
        }
        case SIGUSR2: {
            syslog(LOG_INFO, "disconnect client");
            if (instance.connectionInfo.getPid() == info->si_pid) {
                instance.connectionInfo = ConnectionInfo(0);
            }
            break;
        }
        default: {
            syslog(LOG_INFO, "stop work");
            if (instance.connectionInfo.isAttached()) {
                kill(instance.connectionInfo.getPid(), signum);
                instance.connectionInfo = ConnectionInfo(0);
            }
            instance.work = false;
        }
    }
}

unsigned int Host::getDataFromUser(int end, const std::string &type) {
    int data = -1;
    std::string buf;
    while ((data < 0 || data > end) && work) {
        std::cout << "please enter number of " << type << "( 0 < number <= " << end << " )" << std::endl;
        std::getline(std::cin, buf,'\n');
        try
        {
            data = std::stoi(buf);
        }
        catch (const std::invalid_argument &exp)
        {
            std::cout << "not int input" << std::endl;
            std::cin.clear();
        }
    }
    if(data < 0){
        data = 0;
    }
    return data;
}
