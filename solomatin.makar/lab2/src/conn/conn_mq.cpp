#include <string.h>
#include <mqueue.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "connection.h"
#include "date.h"
#include "global_settings.h"
#include "print_utils.h"

Connection::Connection(int id, bool create) : hostConnection(create), id(id) {
    filename = "/mq" + std::to_string(id);
    if (create) {
        if (mq_unlink(filename.c_str()) == 0) {
            printOk("Message queue " + filename + " removed from system", id);
        }
        mq_attr attr;
        memset(&attr, 0, sizeof(attr));
        attr.mq_msgsize = MSG_SIZE;
        attr.mq_maxmsg = MAX_MSG;
        fd = mq_open(filename.c_str(), O_CREAT | O_RDWR, 0666, &attr);
    } else fd = mq_open(filename.c_str(), O_RDWR);

    if (fd < 0) throw "Could not create/open message queue";

    if (hostConnection) printOk("Created mq " + filename, id);
    else printOk("Connected to mq " + filename, id);
}

Connection::~Connection() {
    printOk("Closing connection..", id);
    mq_close(fd);
    if (hostConnection) mq_unlink(filename.c_str());
    printOk("Connection closed", id);
}

bool Connection::read(char *buffer, int len) {
    char *bigBuffer = new char[MAX_MSG * 2];
    if (mq_receive(fd, bigBuffer, MAX_MSG * 2, nullptr) < 0) {
        perror("mq_receive");
        delete bigBuffer;
        return false;
    }
    memcpy(buffer, bigBuffer, len);
    delete[] bigBuffer;
    return true;
}
bool Connection::write(char *buffer, int len) {
    if (mq_send(fd, buffer, len, 1) < 0) {
        perror("mq_send");
        return false;
    }
    return true;
}

bool Connection::accept() { return true; }