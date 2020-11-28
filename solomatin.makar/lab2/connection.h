#pragma once

class Connection {
    int fd = -1; // may be a port, pipe file id or mq id
public:
    Connection(); // creates new connection
    Connection(int fd); // opens connection with file descriptor

    bool read(char *buffer, int len);
    bool write(char *buffer, int len);
    int getFd() { return fd; }
};