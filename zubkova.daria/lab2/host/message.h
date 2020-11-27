#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

enum class Owner {
    WOLF,
    GOAT
};

enum class Status {
    ALIVE,
    DEAD
};

struct Message {
    Owner owner;
    Status st;
    int num;

    Message(Owner ow = Owner::WOLF, Status st = Status::ALIVE, int num = 0) : owner(ow), st(st), num(num) {}
};


#endif // MESSAGE_H