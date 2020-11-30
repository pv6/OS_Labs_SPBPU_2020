#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "connection.h"
#include "global_settings.h"

Connection::Connection() : fd(-1) {
}

Connection *Connection::create(int id) {
    std::string filename = "/tmp/" + std::to_string(id) + ".fifo";

    if (mkfifo(filename.c_str(), 0777) < 0) {
        throw ("Could not create fifo " + filename).c_str();
    }

    int fd = open(filename.c_str(), O_RDWR);
    if (fd < 0) {
        throw ("Could not open fifo " + filename).c_str();
    }

    sem_t *clientSemaphore = sem_open((CLIENT_SEMAPHORE + std::to_string(id)).c_str(), O_CREAT | O_EXCL, 0666, 1);
    sem_t *serverSemaphore = sem_open((SERVER_SEMAPHORE + std::to_string(id)).c_str(), O_CREAT | O_EXCL, 0666, 1);
    if (clientSemaphore == SEM_FAILED || serverSemaphore == SEM_FAILED) {
        throw "Could not open semaphore";
    }

    Connection *connection = new Connection();
    connection->fd = fd;
    connection->clientSemaphore = clientSemaphore;
    connection->serverSemaphore = serverSemaphore;
    connection->hostConnection = true;
    connection->id = id;
    return connection;
}

Connection *Connection::connect(int id) {
    std::string filename = "/tmp/" + std::to_string(id) + ".fifo";

    int fd = open(filename.c_str(), O_RDWR);
    if (fd < 0) {
        throw ("Could not open fifo " + filename).c_str();
    }

    sem_t *clientSemaphore = sem_open((CLIENT_SEMAPHORE + std::to_string(id)).c_str(), O_RDWR);
    sem_t *serverSemaphore = sem_open((SERVER_SEMAPHORE + std::to_string(id)).c_str(), O_RDWR);
    if (clientSemaphore == SEM_FAILED || serverSemaphore == SEM_FAILED) {
        throw "Could not open semaphore";
    }

    Connection *connection = new Connection();
    connection->fd = fd;
    connection->clientSemaphore = clientSemaphore;
    connection->serverSemaphore = serverSemaphore;
    connection->hostConnection = false;
    connection->id = id;
    return connection;
}

bool Connection::write(char *buffer, int len) {
    sem_t *semaphore = hostConnection ? serverSemaphore : clientSemaphore;

    if (sem_wait(semaphore) < 0) {
        throw "Could not lock semaphore";
    }
    if (::write(fd, buffer, len) < 0) {
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

bool Connection::read(char *buffer, int len) {
    sem_t *semaphore = hostConnection ? serverSemaphore : clientSemaphore;

    if (sem_wait(semaphore) < 0) {
        throw "Could not lock semaphore";
    }
    if (::read(fd, buffer, len) < 0) {
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
    close(fd);

    sem_close(clientSemaphore);
    sem_close(serverSemaphore);

    if (hostConnection) {
        sem_unlink((CLIENT_SEMAPHORE + std::to_string(id)).c_str());
        sem_unlink((SERVER_SEMAPHORE + std::to_string(id)).c_str());
    }
}