#ifndef LAB2_WOLF_H
#define LAB2_WOLF_H

#include <unistd.h>
#include <csignal>
#include <atomic>

#include "../interfaces/Message.h"
#include "GoatInformation.h"

class Wolf {
public:
    static Wolf &getInstance();

    void run(int argc, char* argv[]);
private:
    Wolf();
    ~Wolf();
    Wolf(Wolf const &);
    Wolf &operator=(Wolf const &);

    static const int LIMIT = 100;

    bool openConnection() const;

    void setClientsAmount(int n);

    void terminate(int sig);

    void startWork();

    Message step(Message& ans, GoatInformation& info);

    void getNumber();

    int getClientId();

    static void signalHandler(int sig, siginfo_t* info, void* ptr);

    static void* threadRun(void* data);

    int clientsAmount_;
    int stepNumber_;

    GoatInformation* clientInfo_;

    std::atomic<int> finishAmount_;
    std::atomic<int> step_;

    pthread_cond_t cond_;
    pthread_mutex_t mutex_;

    pthread_t* threads_;
    pthread_attr_t* attr_;
};


#endif //LAB2_WOLF_H
