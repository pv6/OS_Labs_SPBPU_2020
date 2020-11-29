#ifndef LAB2_MESSAGE_H
#define LAB2_MESSAGE_H
#include <stdint.h>

#define SEM_HOST_NAME   "host_semaphore"
#define SEM_CLIENT_NAME "client_semaphore"

struct message
{
    int day, month, year, temp;

    message(int _day = 0, int _month = 0, int _year = 0, int _temp = 0): day(_day), month(_month), year(_year), temp(_temp){}
};

const int MESSAGE_SIZE = sizeof(message);
const int TIMEOUT = 5;
#endif //LAB2_MESSAGE_H
