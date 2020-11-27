#include <signal.h>
#include "date.h"

// common server interface for all connection types
class Server {
    bool channelCreated;
    void createChannel();

    static void handleFunc(int signum, siginfo_t* info, void* ptr);
    Server() : channelCreated(false) {
        struct sigaction action;
        action.sa_sigaction = handleFunc;
        action.sa_flags = SA_SIGINFO;
        sigaction(SIGUSR1, &action, nullptr);
    }

    Server &operator=(const Server&) = delete;
    Server(const Server&) = delete;
public:
    void start(Date date);

    static Server *instance() {
        static Server server;
        return &server;
    }
};