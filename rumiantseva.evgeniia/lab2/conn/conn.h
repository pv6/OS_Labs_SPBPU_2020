//
// Created by Evgenia on 08.11.2020.
//

#ifndef LAB2_CONN_H
#define LAB2_CONN_H

#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <mqueue.h>
#include <cerrno>
#include <syslog.h>
#include <semaphore.h>
#include <csignal>
#include <random>
#include <iostream>
#include <atomic>
#include <random>
#include <limits>
#include <ctime>
#include <pthread.h>


class Conn {
public:
    bool Open(int id, bool create);

    bool Read(void *buf, size_t count);

    bool Write(void *buf, size_t count);

    bool Close();

    Conn();

    ~Conn();

private:
    bool is_created;
    int id;
    std::string connection_name;
    int descr;
    int* f_descr;
};

#endif //LAB2_CONN_H
