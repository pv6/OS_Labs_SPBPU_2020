#include <sys/msg.h>
#include <string.h>
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

struct Message {
    long type;
    char data[sizeof(Date)];
};

Connection::Connection() : fd(-1) {
}

Connection *Connection::create(int id) {
    std::string filename = "/tmp/" + std::to_string(id) + ".mq";
    key_t key = ftok(filename.c_str(), PROJECT_ID);

    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid < 0) {
        throw "Could not create message queue";
    }

    std::string clientSemaphoreName = "/client_semaphore" + std::to_string(id);
    std::string serverSemaphoreName = "/server_semaphore" + std::to_string(id);
    printOk("Trying to create semaphore " + clientSemaphoreName, id);
    printOk("Trying to create semaphore " + serverSemaphoreName, id);
    sem_t *clientSemaphore = sem_open(clientSemaphoreName.c_str(), O_CREAT | O_EXCL, 0666, 1);
    printOk("Created semaphore " + clientSemaphoreName, id);
    sem_t *serverSemaphore = sem_open(serverSemaphoreName.c_str(), O_CREAT | O_EXCL, 0666, 1);
    printOk("Created semaphore " + serverSemaphoreName, id);

    if (clientSemaphore == SEM_FAILED || serverSemaphore == SEM_FAILED) {
        throw "Could not open semaphore";
    }

    Connection *connection = new Connection();
    connection->fd = msgid;
    connection->clientSemaphore = clientSemaphore;
    connection->serverSemaphore = serverSemaphore;
    connection->hostConnection = true;
    connection->id = id;

    return connection;
}

Connection *Connection::connect(int id) {
    std::string filename = "/tmp/" + std::to_string(id) + ".mq";
    key_t key = ftok(filename.c_str(), PROJECT_ID);

    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid < 0) {
        throw "Could not create message queue";
    }

    std::string clientSemaphoreName = "/client_semaphore" + std::to_string(id);
    std::string serverSemaphoreName = "/server_semaphore" + std::to_string(id);

    printOk("Trying to open semaphore " + clientSemaphoreName, id);
    sem_t *clientSemaphore = sem_open(clientSemaphoreName.c_str(), O_RDWR);
    if (clientSemaphore == SEM_FAILED) {
        throw "Could not open semaphore";
    }
    printOk("Semaphore " + serverSemaphoreName + " opened!", id);

    printOk("Trying to open semaphore " + serverSemaphoreName, id);
    sem_t *serverSemaphore = sem_open(serverSemaphoreName.c_str(), O_RDWR);
    if (serverSemaphore == SEM_FAILED) {
        throw "Could not open semaphore";
    }
    printOk("Semaphore " + serverSemaphoreName + " opened!", id);

    Connection *connection = new Connection();
    connection->fd = msgid;
    connection->clientSemaphore = clientSemaphore;
    connection->serverSemaphore = serverSemaphore;
    connection->hostConnection = false;
    connection->id = id;

    return connection;
}

bool Connection::read(char *buffer, int len) {
    Message message;
    sem_t *semaphore = hostConnection ? serverSemaphore : clientSemaphore;

    if (sem_wait(semaphore) < 0) {
        throw "Could not lock semaphore";
    }
    if (msgrcv(fd, &message, sizeof(Message), 1, 0) < 0) {
        if (sem_post(semaphore) < 0) {
            throw "Could not release semaphore";
        }
        return false;
    }

    if (sem_post(semaphore) < 0) {
        throw "Could not release semaphore";
    }
    memcpy(buffer, message.data, len);
    return true;
}

bool Connection::write(char *buffer, int len) {
    Message message;
    message.type = 1;
    memcpy(message.data, buffer, len);

    sem_t *semaphore = hostConnection ? serverSemaphore : clientSemaphore;
    if (sem_wait(semaphore) < 0) {
        throw "Could not lock semaphore";
    }

    if (msgsnd(fd, &message, sizeof(Message), 0) < 0) {
        if (sem_post(semaphore) < 0) {
            throw "Could not release semaphore";
        }
        return false;
    }

    if (sem_post(semaphore) < 0) {
        throw "Could not release semaphore";
    }
    return true;
}

Connection::~Connection() {
    msgctl(fd, IPC_RMID, NULL);
    sem_close(clientSemaphore);
    sem_close(serverSemaphore);

    if (hostConnection) {
        sem_unlink((CLIENT_SEMAPHORE + std::to_string(id)).c_str());
        sem_unlink((SERVER_SEMAPHORE + std::to_string(id)).c_str());
    }
}