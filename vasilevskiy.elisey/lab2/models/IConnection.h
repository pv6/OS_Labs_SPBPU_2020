#ifndef LAB2_ICONNECTION_H
#define LAB2_ICONNECTION_H


#include <cstddef>
#include <string>
#include "DTO.h"

class IConnection {
public:
    void openConnection(size_t id, bool create);

    void readConnection(DTO *buf, size_t size = sizeof(DTO)) const;

    void writeConnection(DTO *buf, size_t size = sizeof(DTO)) const;

    void closeConnection();

private:
    int id;
    bool owner;
    std::string name;
};

#endif //LAB2_ICONNECTION_H
