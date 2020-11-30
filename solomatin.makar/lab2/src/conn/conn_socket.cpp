#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include "connection.h"
#include "global_settings.h"

Connection::Connection() : fd(-1) {
}

Connection *Connection::create(int id) {
    std::string filename = "/tmp/" + std::to_string(id) + ".sock";

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        throw "Could not create socket";
    }

    sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    unlink(filename.c_str());
    strcpy(addr.sun_path, filename.c_str());

    if (bind(fd, (sockaddr *)&addr, sizeof(sockaddr_un)) < 0) {
        throw "Could not bind stream socket";
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
    std::string filename = std::to_string(id) + ".sock";

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        throw "Could not bind stream socket";
    }

    sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, filename.c_str());

    if (::connect(fd, (sockaddr *)&addr, sizeof(sockaddr_un)) < 0) {
        throw "Could not connect to stream socket";
    }

    sem_t *clientSemaphore = sem_open((CLIENT_SEMAPHORE + std::to_string(id)).c_str(), O_RDWR);
    sem_t *serverSemaphore = sem_open((SERVER_SEMAPHORE + std::to_string(id)).c_str(), O_RDWR);
    if (clientSemaphore == SEM_FAILED || serverSemaphore == SEM_FAILED) {
        throw "Could not open semaphore";
    }

    Connection *connection = new Connection();
    connection->fd = fd;
    connection->id = id;
    connection->clientSemaphore = clientSemaphore;
    connection->serverSemaphore = serverSemaphore;
    connection->hostConnection = false;

    return connection;
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