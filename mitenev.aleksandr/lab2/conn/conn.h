//
// Created by aleksandr on 26.11.2020.
//

#ifndef LAB2_CONN_H
#define LAB2_CONN_H


#include <string>
#include <fcntl.h>
#include <syslog.h>
#include "../support/message.h"

class Conn {
public:
    bool openConnection(size_t id, bool create);

    bool readConnection(Message* buf, size_t size = sizeof(Message)) const;

    bool writeConnection(Message *buf, size_t size = sizeof(Message)) const;

    bool closeConnection();

private:
    int id;
    bool owner;
    std::string name;
};
#endif //LAB2_CONN_H
