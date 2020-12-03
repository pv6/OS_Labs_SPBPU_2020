#include "../include/HostManager.h"
#include "../include/ConnectionManager.h"
#include "../include/ClientManager.h"
#include <unistd.h>
#include <pthread.h>
#include <csignal>
#include <iostream>
#include <syslog.h>


HostManager::HostManager() {
}
void HostManager::run() {
    openlog("lab_2", LOG_NDELAY | LOG_PID, LOG_USER);
    syslog(LOG_DEBUG, "HostManager: Run starts");

    _uc.get_clients_number(&_clients_num);

    for (size_t i = 0; i < _clients_num; ++i) {
        ConnectionManager *connection_manager = new ConnectionManager(i);
        syslog(LOG_DEBUG, "HostManager: ConnectionManager created");

        int pid = fork();
        if (pid == 0) {
            syslog(LOG_DEBUG, "HostManager (fork): Forked Process starts");
            ClientManager::create_client_manager(i, connection_manager->get_connector());
            ClientManager *client = ClientManager::get_client_manager();
            syslog(LOG_DEBUG, "HostManager (fork): ClientManager created");

            if (client != nullptr) {
                syslog(LOG_DEBUG, "HostManager (fork): ClientManager run starts");
                client->run();
                syslog(LOG_DEBUG, "HostManager (fork): ClientManager run ends");
            }

            return;
        } else {
            _cms.push_back(connection_manager);

            connection_manager->set_client_pid(pid);

            pthread_t cm_thread;
            syslog(LOG_DEBUG, "HostManager: Thread for ConnectionManager created");
            pthread_create(&cm_thread, NULL, _run_connection_manager, connection_manager);
        }
    }
    syslog(LOG_DEBUG, "HostManager: While cycle for processing Dates starts");
    while (true) {
        _uc.get_date(_date, _date + 1, _date + 2);
        syslog(LOG_DEBUG, "HostManager: New Date received");
        for (auto &&cm : _cms) {
            cm->set_date(_date[0], _date[1], _date[2]);
        }   
        syslog(LOG_DEBUG, "HostManager: New Date passed to ConnectionManagers");
        for (auto &&cm : _cms) {
            if (cm != nullptr) {
                while (cm->check_and_false_date_processed() == false);
            }
        } 
        syslog(LOG_DEBUG, "HostManager: New Date has been processed by all ConnectionManagers");
        if (_uc.check_exit()) {
            syslog(LOG_DEBUG, "HostManager: Exiting starts");
            for (auto &&cm : _cms) {
                cm->finish();
                delete cm;
            }    
            _cms.clear();  
            syslog(LOG_DEBUG, "HostManager: Exiting ends");
            closelog();        
            exit(0);
        }
    }
}
void *HostManager::_run_connection_manager(void *cm) {
    syslog(LOG_DEBUG, "HostManager (thread): ConnectionManager Thread starts");
    ConnectionManager *connection_manager = (ConnectionManager *)cm;
    connection_manager->run();
    syslog(LOG_DEBUG, "HostManager (thread): ConnectionManager Thread ends");
    delete connection_manager;
    return NULL;
}