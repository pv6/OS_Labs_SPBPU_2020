//
// Created by Daria on 11/24/2020.
//

#ifndef WEATHER_HOST_CLIENT_SERVER_H
#define WEATHER_HOST_CLIENT_SERVER_H

#include "../conn/conn.h"
#include "../conn/message.h"
#include "client_handler.h"
#include <signal.h>
#include <vector>

class server {
public:
    static server* get_instance();
    void start();
    void set_num_of_clients(int num);
    std::vector<int> get_date();
    bool is_signalled();
    void signal_got();
private:
    int num_of_clients = 0;
    std::vector<int> date_elems;
    bool signalled_to = false;
    std::vector<client_handler*> client_threads;
    server &operator=(server& s) {
        return s;
    }
    server();
    void create_client(client_handler* handler);
    void send_date_msg(std::vector<int> date_parts);
    static void* execute(void* arg);
    static void signal_handler(int signum, siginfo_t *info, void *ptr);
    bool form_date(std::string date, std::vector<int> &date_elems);
    void terminate(int signum);
    void run();
};

#endif //WEATHER_HOST_CLIENT_SERVER_H
