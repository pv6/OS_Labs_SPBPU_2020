//
// Created by Evgenia on 08.11.2020.
//

#ifndef LAB2_WOLF_H
#define LAB2_WOLF_H

#include "ClientHostConnection.h"


class Wolf {
public:
    void Start();

    bool SignalToClients();

    static Wolf& GetWolfInst();

    void SetGoatsNumber(int n);

    ~Wolf();



    Wolf(const Wolf &) = delete;
    Wolf &operator=(const Wolf &) = delete;

private:
    int goats_number = 0;
    bool is_running = true;

    ClientHostConnection* connections_to_clients = nullptr;
    pthread_t* clients_threads = nullptr;

    std::atomic<int> current_answer;
    std::atomic<int> goats_answered;
    std::atomic<int> current_step;
    pthread_cond_t cond_var;
    pthread_mutex_t mutex;

    void Terminate(int signum);
    int Decide(int goat_answer, ClientHostConnection &info);

    Wolf();
    void GenerateNumber();

    int GenerateClientId();

    static void* Thread(void* arg);

    static void SignalHandler(int signum, siginfo_t *info, void *ptr);

};

#endif //LAB2_WOLF_H
