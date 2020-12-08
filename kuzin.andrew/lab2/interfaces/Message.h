#ifndef LAB2_MESSAGE_H
#define LAB2_MESSAGE_H

#define TIMEOUT 5

enum class Status {
    ALIVE,
    DEAD
};

struct Message {
    Status status_;
    int number_;

    Message(Status st = Status::ALIVE, int num = 0) : status_(st), number_(num) {}
};


#endif //LAB2_MESSAGE_H
