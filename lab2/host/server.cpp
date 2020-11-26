//
// Created by Daria on 11/24/2020.
//
#include "server.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <syslog.h>
#include <vector>
#include <sstream>


server* server::get_instance() {
    static server self;
    return &self;
}

void server::set_num_of_clients(int num) {
    num_of_clients = num;
    conn_w_clients.clear();
    conn_w_clients = std::vector<client_host_connection*>();
    client_threads.clear();
    client_threads = std::vector<client_handler*>();
}

server::server() {
    struct sigaction act{};
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = signal_handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
}

void server::create_client(client_handler* handler) {
    int client_pid;
    client_pid = fork();
    if (client_pid == 0) {
        future_weather* client = future_weather::get_instance(handler->get_id());
        client->set_conn(handler->get_conn());
        if(!client->open_connection()) {
            syslog(LOG_ERR, "Impossible to connect handler and client");
            return;
        }
        client->start();
        syslog(LOG_INFO, "Client created");
        return;
    }
    handler->set_client(client_pid);
}

void* server::execute(void *arg) {
    auto* handler = (client_handler*)arg;
    handler->start();
    return nullptr;
}

void server::start() {
    syslog(LOG_INFO, "Creation of all clients started in server");
    auto *conn = new client_host_connection();
    for (int i = 0; i < num_of_clients; i++) {
        auto *handler = new client_handler(i);
        conn->open(handler->get_id());
        handler->set_conn_w_server(conn);
        if(!handler->open_connection_w_weather()) {
            syslog(LOG_ERR, "Error in connection establishment: %s", strerror(errno));
            return;
        }
        create_client(handler);
        client_threads.push_back(handler);
        conn_w_clients.push_back(conn);

        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        handler->set_tid(tid);
        void *(*function)(void *) = execute;
        pthread_create(&tid, &attr, function, handler);
    }
    run();
}

void server::signal_handler(int signum, siginfo_t *info, void *ptr) {
    static server* instance = get_instance();
    switch (signum)
    {
        case SIGTERM:
        {
            instance->terminate(signum);
            break;
        }
    }
}

void server::terminate(int signum) {
    syslog(LOG_INFO, "Starting server's disconnection");
    for (int i = 0; i < num_of_clients; i++) {
        conn_w_clients[i]->disconnect();
    }
    closelog();
    exit(signum);
}

bool server::form_date(std::string date, std::vector<int> &date_elems) {
    char delim = '.';
    std::stringstream str_stream(date);
    std::string item;
    while(std::getline(str_stream, item, delim)) {
        date_elems.push_back(std::stoi(item));
    }
    if (date_elems.size() < 3) {
        syslog(LOG_ERR, "Incorrect date: DD.MM.YYYY needed!");
        return false;
    }
    int year = date_elems.at(2);
    if (year <= 0) {
        syslog(LOG_ERR, "Incorrect date: negative year!");
        return false;
    }
    int month = date_elems.at(1);
    if (month <= 0 || month > 12) {
        syslog(LOG_ERR, "Incorrect date: month does not exist!");
        return false;
    }

    int day = date_elems.at(0);
    int days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year % 4 == 0)
        days_in_month[1] = 29;
    if (day <= 0 || day > days_in_month[month - 1]) {
        syslog(LOG_ERR, "Incorrect date: confusing day!");
        return false;
    }
    return true;
}

bool server::msg_from_date(std::string date, message &msg) {
    std::vector<int> date_elems;
    if (!form_date(date, date_elems)) {
        syslog(LOG_ERR, "Data forming failed");
        return false;
    }
    msg.set_day(date_elems.at(0));
    msg.set_month(date_elems.at(1));
    msg.set_year(date_elems.at(2));
    return true;
}

void server::run() {
    std::string new_date;
    message msg;
    bool flag_start = true;
    std::vector<int> date_elems;
    while (true) {
        new_date = "";
        flag_start = true;
        date_elems.clear();
        while (!form_date(new_date, date_elems)) {
            if (std::cin.fail()) {
                std::cin.clear();
                new_date = "";
            } else {
                if (flag_start)
                    flag_start = false;
                else
                    std::cout << "Incrorrect format: try again!" << std::endl;
                std::cout << "Enter the date in format: DD.MM.YYYY OR enter \"q\" for exit."<< std::endl;
            }
            std::cin >> new_date;
            if (new_date == "q" || new_date == "Q") {
                syslog(LOG_INFO, "Terminating server...");
                terminate(EXIT_SUCCESS);
            }
        }

        msg_from_date(new_date, msg);
        send_date_msg(msg);
    }
}

void server::send_date_msg(message msg) {
    for (int i = 0; i < num_of_clients; i++) {
        if (!conn_w_clients[i]->is_closed()) {
            conn_w_clients[i]->write(&msg, sizeof(msg));
            conn_w_clients[i]->signal_to();
        }
    }
}