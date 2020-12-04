#include <iostream>
#include <sstream>
#include <vector>
#include <syslog.h>
#include "Host.h"

Host& Host::getInstance(int pid) {
    static Host instance(pid);
    return instance;
}

void Host::openConnection(sem_t* sem_host, sem_t *sem_client) {
    syslog(LOG_INFO, "Host open connection");
    connection.openConnection(connInfo.getPid(), true);
    semaphore_host = sem_host;
    semaphore_client = sem_client;
}

bool Host::inputDate() {
    std::string cur_num;
    std::string date_str;
    std::cin >> date_str;
    std::istringstream stringStream;
    stringStream.str(date_str);
    std::vector<unsigned int> nums;
    while (std::getline(stringStream, cur_num, '.')) {
        try {
            unsigned int num = std::stoul(cur_num);
            nums.push_back(num);
        }
        catch (...) {
            std::cout << "Not number in date" << std::endl;
            return false;
        }
    }
    if (nums.size() != 3) {

    }
    tm date{};
    work = TurnOff(nums[0],nums[1], nums[2]);
    date.tm_year = int(nums[2]) - 1900;
    date.tm_mon = int(nums[1]) - 1;
    date.tm_mday = nums[0];
    date.tm_isdst = -1;
    tm tCheck = date;
    mktime(&tCheck);
    if ((date.tm_year != tCheck.tm_year || 
            date.tm_mon != tCheck.tm_mon || 
                date.tm_mday != tCheck.tm_mday) && work)
    {
        std::cout << "Wrong date" << std::endl;
        return false;
    }
    message = DateAndTemp(nums[0], nums[1], nums[2]);
    return true;
}

void Host::run() {
    timespec ts{};
    syslog(LOG_INFO, "wait client connect");
    syslog(LOG_INFO, "client attached");
    work = true;
    sem_post(semaphore_client);
    while (work) {
        if (!work) {
            connInfo = ConnectionInfo(0);
            return;
        }
        while(!inputDate());
        if (connInfo.isAttached()){
            connection.writeConnection(&message);
        } else {
            work = false;
            continue;
        }
        syslog(LOG_INFO, "call client");
        sem_post(semaphore_client);
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout;
        syslog(LOG_INFO, "wait client data");
        if (sem_timedwait(semaphore_host, &ts) == -1) {
            syslog(LOG_INFO, "timeout");
            syslog(LOG_INFO, "try turn off client");
            kill(connInfo.getPid(), SIGTERM);
            connInfo = ConnectionInfo(0);
            continue;
        }
        syslog(LOG_INFO, "working");
        connection.readConnection(&message);
        printDateAndTemp();
    }
}

void Host::terminate() {
    connection.closeConnection();
}

void Host::printDateAndTemp() {
    std::cout << "D: " << message.getDay() << "." << message.getMonth() << "." << message.getYear() << " T: " << message.getTemp() << std::endl;
}

Host::Host(int pid) : connInfo(pid) {
    message = DateAndTemp(0, 0, 0);
}

bool Host::TurnOff(unsigned day, unsigned month, unsigned year) {
    unsigned sum = day + month + year;
    return sum != 0;
}
