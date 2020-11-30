//
// Created by Daria on 11/24/2020.
//

#ifndef WEATHER_HOST_CLIENT_CLIENT_HOST_CONNECTION_H
#define WEATHER_HOST_CLIENT_CLIENT_HOST_CONNECTION_H

#include "../conn/conn.h"
#include <pthread.h>


class client_host_connection {
public:
    bool disconnect();
    bool open(int id);
    bool read(void *buf, size_t size);
    bool write(void *buf, size_t size);
    int get_client_id();
    bool is_closed() const { return is_closed_; };
    void signal_to();
    bool has_signalled_to() const;
private:
    bool signalled_to = false;
    int client_id;
    bool is_closed_ = true;
    int fd[2];
};

#endif //WEATHER_HOST_CLIENT_CLIENT_HOST_CONNECTION_H
