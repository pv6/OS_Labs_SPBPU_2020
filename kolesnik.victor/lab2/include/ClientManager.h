#ifndef CLIENTMANAGER_H_INCLUDED__
#define CLIENTMANAGER_H_INCLUDED__

#include "../include/WeatherForecaster.h"
#include "../include/Conn.h"
#include <stddef.h>
#include <semaphore.h>
#include <string>

class ClientManager {
public:
    void run();
    
    static void create_client_manager(size_t client_id, Conn &connector);
    static ClientManager *get_client_manager();
    static void signal_handler(int signal);

private:
    size_t _id;
    WeatherForecaster _wf;
    Conn _connector;

    sem_t *_semaphore_to_read_date;
    sem_t *_semaphore_to_write_temperature;

    std::string to_write;
    std::string to_read;

    bool _working;

    ClientManager(size_t client_id, Conn &connector);

    void _finish();
};

#endif