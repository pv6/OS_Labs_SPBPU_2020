//
// Created by Daria on 11/24/2020.
//

#ifndef WEATHER_HOST_CLIENT_SERVER_H
#define WEATHER_HOST_CLIENT_SERVER_H

#include "../conn/conn.h"
#include "../conn/message.h"
#include "client_host_connection.h"
#include "client_handler.h"
#include <signal.h>
#include <vector>

class server {
public:
    static server* get_instance();
    void start();
    void set_num_of_clients(int num);
    server &operator=(server& s) {
        return s;
    }
private:
    int num_of_clients = 0;
    std::vector<client_host_connection*> conn_w_clients;
    std::vector<client_handler*> client_threads;
    //client_host_connection* conn_w_clients = nullptr;
    //client_handler* client_threads = nullptr;
    server();
    void create_client(client_handler* handler);
    void send_date_msg(message msg);
    static void* execute(void* arg);
    static void signal_handler(int signum, siginfo_t *info, void *ptr);
    void terminate(int signum);
    bool form_date(std::string date, std::vector<int> &date_elems);
    void run();
    bool msg_from_date(std::string date, message &msg);
};

#endif //WEATHER_HOST_CLIENT_SERVER_H
