#ifndef USERCOMMUNION_H_INCLUDED__
#define USERCOMMUNION_H_INCLUDED__

#include <stddef.h>

class UserCommunion {
public:
    UserCommunion() = default;

    void get_clients_number(size_t *n);
    void get_date(int *day, int *month, int *year);
    bool check_exit();
    void put_forecast(int client_id, int temperature);
};

#endif