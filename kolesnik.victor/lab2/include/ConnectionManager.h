#ifndef CONNECTIONMANAGER_H_INCLUDED__
#define CONNECTIONMANAGER_H_INCLUDED__

#include "../include/Conn.h"
#include "../include/UserCommunion.h"
#include <stddef.h>
#include <semaphore.h>
#include <string>

class ConnectionManager {
public:
    ConnectionManager(size_t client_id);

    Conn &get_connector();
    void set_client_pid(size_t client_pid);
    void set_date(int day, int month, int year);
    bool check_and_false_date_processed();

    void run();
    void finish();

private:
    size_t _client_id;
    size_t _client_pid;

    UserCommunion _uc;
    Conn _connector;

    int _date[3];
    bool _new_date;

    bool _working;
    bool _date_processed;

    sem_t *_semaphore_to_read_temperature;
    sem_t *_semaphore_to_write_date;

    std::string to_write;
    std::string to_read;

    const size_t timeout = 5;
};

#endif