#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include "connection.h"
#include "global_settings.h"
#include "print_utils.h"

Connection::Connection(int id, bool create) : hostConnection(create), id(id), acceptFd(-1) {
    filename = "/tmp/" + std::to_string(id) + ".sock";

    if (create && unlink(filename.c_str()) == 0) {
        printOk("Deleted sock file: " + filename, id);
    };

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) throw "Could not create socket";

    sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, filename.c_str());

    if (hostConnection) {
        printOk("Trying to bind " + filename, id);
        if (bind(fd, (sockaddr *)&addr, sizeof(sockaddr_un)) < 0) {
            perror("");
            throw "Could not bind socket";
        }
        printOk("Socket bound " + filename, id);

        printOk("Trying to start listening", id);
        if (listen(fd, 10) < 0) {
            perror("");
            throw "Could not start listening";
        }
    } else {
        if (connect(fd, (sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("");
            throw "Could not connect to server";
        }
    }
}

Connection::~Connection() {
    printOk("Closing connection..", id);
    close(fd);
    printOk("Connection closed", id);
}

bool Connection::write(char *buffer, int len) {
    if (hostConnection) {
        if (acceptFd == -1) return false;
        return ::write(acceptFd, buffer, len) >= 0;
    }
    return ::write(fd, buffer, len) >= 0;
}

bool Connection::read(char *buffer, int len) {
    int readFd = hostConnection ? acceptFd : fd;
    if (readFd == -1) return false;

    if (::read(readFd, buffer, len) < 0) {
        perror("read");
        return false;
    }
    return true;
}

bool Connection::accept() {
    acceptFd = ::accept(fd, nullptr, nullptr);
    return acceptFd >= 0;
}