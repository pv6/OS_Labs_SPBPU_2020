//
// Created by Daria on 11/24/2020.
//

#ifndef WEATHER_HOST_CLIENT_FUTURE_WEATHER_H
#define WEATHER_HOST_CLIENT_FUTURE_WEATHER_H

#include "../conn/message.h"
#include "../conn/conn.h"
#include <semaphore.h>

class future_weather {
public:
    static future_weather *get_instance(int host_pid);
    void start();
    bool open_connection();
    void set_conn(conn connection);
private:
    conn connection;
    sem_t *semaphore_host;
    sem_t *semaphore_client;
    std::string sem_client_name;
    std::string sem_host_name;

    int host_pid;
    int rand_offset;

    future_weather(int host_pid);
    future_weather(future_weather &weather);
    future_weather &operator=(future_weather &weather);
    void terminate(int signum);
    int get_weather(int day, int month, int year);
    static void signal_handler(int signum);
};
#endif //WEATHER_HOST_CLIENT_FUTURE_WEATHER_H
