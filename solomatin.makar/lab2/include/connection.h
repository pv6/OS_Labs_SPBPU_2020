#pragma once

class Connection {
    int fd = -1; // may be a port, pipe file id or mq id
    double lifetime = 0;
public:
    Connection(); // creates new connection
    Connection(int fd); // opens connection with file descriptor

    bool read(char *buffer, int len);
    bool write(char *buffer, int len);
    int getFd() { return fd; }
    void increaseLifetime(double delta) {
        if (delta > 0) { this->lifetime += delta; }
    }
    double getLifetime() { return lifetime; }
};