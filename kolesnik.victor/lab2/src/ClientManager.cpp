#include "../include/ClientManager.h"
#include <string>
#include <csignal>
#include <syslog.h>
#include <unistd.h>


static ClientManager *_cm = nullptr;


void ClientManager::create_client_manager(size_t client_id, Conn &connector) {
    if (_cm == nullptr) {
        _cm = new ClientManager(client_id, connector);
        syslog(LOG_DEBUG, "ClientManager (%d): ClientManager created", (int)client_id);
    }
}
ClientManager *ClientManager::get_client_manager() {
    return _cm;
}
ClientManager::ClientManager(size_t client_id, Conn &connector)
    : _id(client_id), _wf(client_id), _connector(connector), _working(true)
{
    to_read = "host_to_client_" + std::to_string(client_id) + "_";
    to_write = "client_to_host_" + std::to_string(client_id) + "_";

    _semaphore_to_read_date = sem_open(to_read.c_str(), 0);
    _semaphore_to_write_temperature = sem_open(to_write.c_str(), 0);

    signal(SIGTERM, signal_handler);
}
void ClientManager::run() {
    syslog(LOG_DEBUG, "ClientManager (%d): Run starts", (int)_id);
    int date[3];

    while (_working) {
        sem_wait(_semaphore_to_read_date);
        syslog(LOG_DEBUG, "ClientManager (%d): New Date added", (int)_id);
        if (_connector.read(date, 3 * sizeof(int))) {
            syslog(LOG_DEBUG, "ClientManager (%d): New Date received", (int)_id);
            int temperature = _wf.forecast(date[0], date[1], date[2]);
            _connector.write(&temperature, sizeof(int));
            syslog(LOG_DEBUG, "ClientManager (%d): Temperature written", (int)_id);
            sem_post(_semaphore_to_write_temperature);
        }
    }
    syslog(LOG_DEBUG, "ClientManager (%d): Run ends", (int)_id);
}
void ClientManager::_finish() {
    if (_working) {
        syslog(LOG_DEBUG, "ClientManager (%d): Finishing starts", (int)_id);

        sem_unlink(to_read.c_str());
        sem_unlink(to_write.c_str());

        _semaphore_to_write_temperature = SEM_FAILED;
        _semaphore_to_read_date = SEM_FAILED;

        _working = false;
        syslog(LOG_DEBUG, "ClientManager (%d): Finishing ends", (int)_id);

        _connector.~Conn();

        exit(0);
    }
}
void ClientManager::signal_handler(int signal) {
    switch (signal) {
        case SIGTERM:
            syslog(LOG_DEBUG, "ClientManager (%d): Signal SIGTERM received", (int)get_client_manager()->_id);
            get_client_manager()->_finish();
            delete _cm;
            _cm = nullptr;
            break;
        default:
            break;
    }
}