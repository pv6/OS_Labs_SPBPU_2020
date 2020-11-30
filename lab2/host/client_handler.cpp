//
// Created by Daria on 11/24/2020.
//

#include "client_handler.h"
#include "server.h"
#include <fcntl.h>
#include <cstring>
#include <csignal>
#include <semaphore.h>
#include <syslog.h>
#include <iostream>

client_handler::client_handler(int id) {
    client_id = id;
}

client_handler& client_handler::operator=(client_handler& c) {
    return c;
}

void client_handler::set_client(int pid) {
    client_pid = pid;
    syslog(LOG_INFO, "Setting client w/ pid: %d", pid);
    attached = (client_pid != 0);
}

conn client_handler::get_conn() {
    return conn_w_weather;
}

bool client_handler::open_connection_w_weather() {
    if (conn_w_weather.open(client_id, true)) {
        sem_client_name = "client_" + std::to_string(client_id);
        sem_host_name = "host_" + std::to_string(client_id);

        semaphore_host = sem_open(sem_host_name.c_str(), O_CREAT, 0666, 0);
        if (semaphore_host == SEM_FAILED) {
            syslog(LOG_ERR, "ERROR: handler: can`t open host semaphore error = %s", strerror(errno));
            return false;
        }
        syslog(LOG_NOTICE, "handler: host semaphore created (%s)", sem_host_name.c_str());

        semaphore_client = sem_open(sem_client_name.c_str(), O_CREAT, 0666, 0);
        if (semaphore_client == SEM_FAILED) {
            syslog(LOG_ERR, "ERROR: handler: can`t open client semaphore error = %s", strerror(errno));
            return false;
        }
        syslog(LOG_NOTICE, "handler: client semaphore created (%s)", sem_client_name.c_str());
        return true;
    }
    return false;
}

void client_handler::set_tid(pthread_t tid) {
    tid_ = tid;
}

pthread_t client_handler::get_tid() {
    return tid_;
}

int client_handler::get_id() {
    return client_id;
}

void client_handler::kill_client() {
    if (attached) {
        syslog(LOG_NOTICE, "Kill client");
        kill(client_pid, SIGTERM);
    }
}

void client_handler::terminate(int signum) {
    if (closed)
        return;
    closed = true;
    syslog(LOG_NOTICE, "Termination of handler w/ id %d", client_id);
    kill_client();

    if (!conn_w_weather.close()) {
        syslog(LOG_ERR, "Terminating error connection: %s", strerror(errno));
    }
    if (semaphore_host != SEM_FAILED) {
        semaphore_host = SEM_FAILED;
        if (sem_unlink(sem_host_name.c_str()) != 0) {
            syslog(LOG_ERR, "Terminating error sem_host_name: %s", strerror(errno));
        }
    }

    if (semaphore_client != SEM_FAILED) {
        semaphore_client = SEM_FAILED;
        if (sem_unlink(sem_client_name.c_str()) != 0) {
            syslog(LOG_ERR, "Terminating error sem_client_name: %s", strerror(errno));
        }
    }
    syslog(LOG_NOTICE, "Handler %d was killed", client_id);
}

bool client_handler::get_date_msg(message &msg) {
    server* cur_server = server::get_instance();
    if (!cur_server) {
        syslog(LOG_ERR, "Impossible to reach server");
        return false;
    }
    std::vector<int> date_elems = cur_server->get_date();
    msg_from_date(date_elems, msg);
    return true;
}

void client_handler::start() {
    struct timespec ts;
    int res;

    message msg;
    message buf;
    server* cur_server = server::get_instance();
    if (!cur_server) {
        syslog(LOG_ERR, "Impossible to reach server");
        return;
    }
    while (true) {
        if (cur_server->is_signalled()) {
            get_date_msg(msg);
            cur_server->signal_got();
            std::cout << "Message got! ";
            if (conn_w_weather.write(&msg, sizeof(msg))) {
                std::cout << "Wait for response..." << std::endl;
                sem_post(semaphore_client);
                clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_sec += timeout * 1000;
                res = sem_timedwait(semaphore_host, &ts);
                if (res == -1) {
                    kill_client();
                    terminate(errno);
                    return;
                } else {
                    if (conn_w_weather.read(&buf, sizeof(buf))) {
                        std::cout << "ANSWER: date = " << msg.get_day() << "." << msg.get_month() << "."
                                  << msg.get_year();
                        std::cout << ", RESPONSE: temperature = " << buf.get_temp() << std::endl;
                    }
                }
            }
        }
    }
}



void client_handler::msg_from_date(std::vector<int> date_elems, message &msg) {
    msg.set_day(date_elems.at(0));
    msg.set_month(date_elems.at(1));
    msg.set_year(date_elems.at(2));
}
