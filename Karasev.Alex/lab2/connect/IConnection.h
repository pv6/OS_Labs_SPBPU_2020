#ifndef LAB2_ICONNECTION_H
#define LAB2_ICONNECTION_H


#include <cstddef>
#include <string>
#include "DateAndTemp.h"

class IConnection {
public:
    void openConnection(size_t id, bool create);

    void readConnection(DateAndTemp *buf, size_t size = sizeof(DateAndTemp)) const;

    void writeConnection(DateAndTemp *buf, size_t size = sizeof(DateAndTemp)) const;

    void closeConnection() const;

private:
    int id;
    bool owner;
    std::string name;
};

#endif //LAB2_ICONNECTION_H