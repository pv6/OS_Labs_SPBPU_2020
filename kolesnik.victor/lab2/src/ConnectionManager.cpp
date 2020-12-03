#include "../include/ConnectionManager.h"
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <csignal>
#include <syslog.h>


ConnectionManager::ConnectionManager(size_t client_id)
    : _client_id(client_id), _uc(), _connector(client_id), _new_date(false), _working(true), _date_processed(false)
{
    to_write = "host_to_client_" + std::to_string(client_id) + "_";
    to_read = "client_to_host_" + std::to_string(client_id) + "_";

    _semaphore_to_read_temperature = sem_open(to_read.c_str(), O_CREAT, S_IRWXU, 0);
    _semaphore_to_write_date = sem_open(to_write.c_str(), O_CREAT, S_IRWXU, 0);

    syslog(LOG_DEBUG, "ConnectionManager (%d): ConnectionManager created", (int)_client_id);
}
Conn &ConnectionManager::get_connector() {
    return _connector;
}
void ConnectionManager::set_client_pid(size_t client_pid) {
    _client_pid = client_pid;
}
void ConnectionManager::set_date(int day, int month, int year) {
    _date[0] = day;
    _date[1] = month;
    _date[2] = year;

    syslog(LOG_DEBUG, "ConnectionManager (%d): New Date set", (int)_client_id);

    _new_date = true;
}
bool ConnectionManager::check_and_false_date_processed() {
    if (_date_processed) {
        _date_processed = false;
        return true;
    } else {
        return false;
    }
}
void ConnectionManager::run() {
    syslog(LOG_DEBUG, "ConnectionManager (%d): Run starts", (int)_client_id);
    struct timespec wait_time;
    int temperature;

    while(_working) {
        if (_new_date == true) {
            syslog(LOG_DEBUG, "ConnectionManager (%d): New Date received", (int)_client_id);
            _new_date = false;
            
            if (_connector.write(_date, 3 * sizeof(int))) {
                syslog(LOG_DEBUG, "ConnectionManager (%d): New Date written", (int)_client_id);

                sem_post(_semaphore_to_write_date);

                clock_gettime(CLOCK_REALTIME, &wait_time);
                wait_time.tv_sec += timeout * 1000;

                if (sem_timedwait(_semaphore_to_read_temperature, &wait_time) == 0) {
                    if (_connector.read(&temperature, sizeof(int))) {
                        syslog(LOG_DEBUG, "ConnectionManager (%d): Temperature gotten", (int)_client_id);
                        _uc.put_forecast(_client_id, temperature);
                        _date_processed = true;
                    }
                } else {
                    syslog(LOG_DEBUG, "ConnectionManager (%d): Waittime expired", (int)_client_id);

                    finish();

                    return;
                }
            }
        }
    }
    syslog(LOG_DEBUG, "ConnectionManager (%d): Run ends", (int)_client_id);
}
void ConnectionManager::finish() {
    if (_working) {
        syslog(LOG_DEBUG, "ConnectionManager (%d): Finishing starts", (int)_client_id);
        _working = false;

        sem_unlink(to_read.c_str());
        sem_unlink(to_write.c_str());

        _semaphore_to_read_temperature = SEM_FAILED;
        _semaphore_to_write_date = SEM_FAILED;

        syslog(LOG_DEBUG, "ConnectionManager (%d): Client kill signal is sent", (int)_client_id);
        kill(_client_pid, SIGTERM);

        syslog(LOG_DEBUG, "ConnectionManager (%d): Finishing ends", (int)_client_id);
    }
}