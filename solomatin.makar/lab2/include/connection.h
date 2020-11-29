#pragma once

#include <string>

class Connection {
    int id = -1; // may be a port, pipe file id or mq id
    double lifetime = 0;

    const int projId = 10;
    const std::string filePrefix = "conn";
public:
    Connection(int id); // creates/opens connection with file descriptor
    ~Connection(); // closes connection

    bool read(char *buffer, int len);
    bool write(char *buffer, int len);
    int getId() { return id; }
    void increaseLifetime(double delta) {
        if (delta > 0) { this->lifetime += delta; }
    }
    double getLifetime() { return lifetime; }
};