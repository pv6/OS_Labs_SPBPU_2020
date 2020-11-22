//
// Created by Evgenia on 08.11.2020.
//

#include "Goat.h"

void Goat::Start() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned> distribution_alive(1, 100);
    std::uniform_int_distribution<unsigned> distribution_dead(1, 50);
    int number = distribution_alive(gen);
    connection.Write(&number, sizeof(int));
    syslog(LOG_NOTICE, "Goat %i picks number %i", client_id, number);
    sem_post(semaphore_host);
    int result;
    while (is_running) {
        sem_wait(semaphore);
        syslog(LOG_NOTICE, "w");
        if (connection.Read(&result, sizeof(int))) {
            if (result) {
                number = distribution_alive(gen);
                syslog(LOG_NOTICE, "Goat %i is alive", client_id);
            } else {
                number = distribution_dead(gen);
                syslog(LOG_NOTICE, "Goat %i is dead", client_id);
            }
            connection.Write(&number, sizeof(int));
            syslog(LOG_NOTICE, "Goat %i picks number %i", client_id, number);
        }
        sem_post(semaphore_host);
    }
}

bool Goat::SetupConnection() {
    if (!host_pid)
        return false;
    alarm(5);
    kill(host_pid, SIGUSR1);
    while (client_id < 0)
        pause();

    if (connection.Open(client_id, false)) {
        std::string semName = 's' + std::to_string(client_id);
        std::string semName2 = 'h' + std::to_string(client_id);
        semaphore = sem_open(semName.c_str(), 0);
        semaphore_host = sem_open(semName2.c_str(), 0);
        if (semaphore == SEM_FAILED || semaphore_host == SEM_FAILED) {
            syslog(LOG_ERR, "%s", strerror(errno));
            return false;
        } else {
            syslog(LOG_NOTICE, "Connection %i is set", client_id);
            return true;
        }
    }
    return false;
}

void Goat::SetHostPid(int pid) {
    host_pid = pid;
}

Goat& Goat::GetGoatInst() {
    static Goat inst;
    return inst;
}

void Goat::Terminate(int signum) {
    kill(host_pid, SIGUSR2);
    if (sem_close(semaphore) == -1 || sem_close(semaphore_host) == -1) {
        syslog(LOG_ERR, "%s", strerror(errno));
        exit(errno);
    }
    if (!connection.Close()) {
        syslog(LOG_ERR, "%s", strerror(errno));
        exit(errno);
    }
    syslog(LOG_NOTICE, "Goat %i is terminated", client_id);
    is_running = false;
}

Goat::Goat() {
    host_pid = 0;
    client_id = -1;

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = SignalHandler;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGUSR1, &act, nullptr);
    sigaction(SIGALRM, &act, nullptr);
}

void Goat::SignalHandler(int signum, siginfo_t* si, void* ucontext) {
    Goat &instance = Goat::GetGoatInst();
    switch (signum) {
        case SIGUSR1: {
            instance.client_id = si->si_value.sival_int;
            break;
        }
        case SIGALRM: {
            if (instance.client_id + 1 > 0)
                break;
        }
        default: {
            instance.Terminate(signum);
        }
    }
}
