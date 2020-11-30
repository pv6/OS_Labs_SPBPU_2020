//
// Created by Daria on 11/24/2020.
//

#ifndef WEATHER_HOST_CLIENT_CLIENT_HANDLER_H
#define WEATHER_HOST_CLIENT_CLIENT_HANDLER_H

#include "../conn/message.h"
#include "future_weather.h"
#include <semaphore.h>
#include <vector>


class client_handler {
public:
    client_handler() = default;
    client_handler(int id);
    void set_client(int pid);
    void start();
    bool open_connection_w_weather();
    void set_tid(pthread_t tid);
    pthread_t get_tid();
    int get_id();
    conn get_conn();
    client_handler(client_handler& c) = default;
private:
    int client_id;
    int client_pid;
    bool attached;
    bool closed;
    conn conn_w_weather;
    sem_t *semaphore_host;
    sem_t *semaphore_client;

    std::string sem_client_name;
    std::string sem_host_name;

    pthread_t tid_;

    client_handler& operator=(client_handler& c);
    ~client_handler() {
        terminate(EXIT_SUCCESS);
    }

    void msg_from_date(std::vector<int> date_elems, message &msg);

    bool get_date_msg(message &msg);


    void kill_client();

    void terminate(int signum);

};
#endif //WEATHER_HOST_CLIENT_CLIENT_HANDLER_H
