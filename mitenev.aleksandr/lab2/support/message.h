//
// Created by aleksandr on 26.11.2020.
//

#ifndef LAB2_MESSAGE_H
#define LAB2_MESSAGE_H


enum class GOAT_STATUS
{
    ALIVE = 0,
    DEAD
};

struct Message {
    GOAT_STATUS status;
    int number;
};


#endif //LAB2_MESSAGE_H
