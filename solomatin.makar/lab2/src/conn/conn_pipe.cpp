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
#include "print_utils.h"

Connection::Connection(int id, bool create) : hostConnection(create), id(id) {
    filename = "/tmp/" + std::to_string(id) + ".fifo";
    if (create) {
        if (unlink(filename.c_str()) == 0) {
            printOk("Deleted pipe " + filename, id);
        }
        if (mkfifo(filename.c_str(), 0666) < 0) {
            throw ("Could not create fifo " + filename).c_str();
        }
    }
    if ((fd = open(filename.c_str(), O_RDWR)) < 0) {
        throw ("Could not open fifo " + filename).c_str();
    }

    if (hostConnection) printOk("Created pipe " + filename, id);
    else printOk("Opened pipe " + filename, id);
}

bool Connection::write(char *buffer, int len) {
    if (::write(fd, buffer, len) < 0) {
        perror("write");
        return false;
    }
    return true;
}

bool Connection::read(char *buffer, int len) {
    if (::read(fd, buffer, len) < 0) {
        perror("read");
        return false;
    }
    return true;
}

Connection::~Connection() {
    printOk("Closing connection..", id);
    close(fd);
    printOk("Connection closed", id);
}

bool Connection::accept() { return true; }