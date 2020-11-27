#pragma once

class Connection {
    int fd = -1;
public:
    Connection(); // creates new connection
    Connection(int fd); // opens connection with file descriptor

    bool read(char *buffer, int len);
    bool write(char *buffer, int len);
    int getFd() { return fd; }
};