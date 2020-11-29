#include <sys/msg.h>
#include <string.h>
#include <string>
#include "connection.h"
#include "date.h"

Connection::Connection(int id) {
    const char *fileName = (filePrefix + std::to_string(id)).c_str();
    key_t key = ftok(fileName, projId);

    this->id = msgget(key, 0666 | IPC_CREAT);
}

bool Connection::read(char *buffer, int len) {
    char *message = new char[sizeof(long) + len];
    msgrcv(id, message, sizeof(long) + len, 1, 0);

    memcpy(buffer, message + sizeof(long), len);

    return true;
}

bool Connection::write(char *buffer, int len) {
    char *message = new char[sizeof(long) + len];
    *(long *)message = 1;
    memcpy(buffer + sizeof(long), buffer, len);

    msgsnd(id, message, sizeof(long) + len, 0);
    delete message;

    return true;
}

Connection::~Connection() {
    msgctl(id, IPC_RMID, NULL);
}