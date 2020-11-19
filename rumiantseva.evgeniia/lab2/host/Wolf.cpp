//
// Created by Evgenia on 08.11.2020.
//

#include "Wolf.h"


Wolf::Wolf() {
    umask(0);
    current_answer = 0;
    goats_answered = 0;
    current_step = 0;

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = SignalHandler;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGINT, &act, nullptr);
    sigaction(SIGUSR1, &act, nullptr);
    sigaction(SIGUSR2, &act, nullptr);

    pthread_cond_init(&cond_var, NULL);
    pthread_mutex_init(&mutex, NULL);
}


Wolf& Wolf::GetWolfInst() {
    static Wolf inst;
    return inst;
}


bool Wolf::SignalToClients() {
    bool res;
    for (int i = 0; i < goats_number; ++i) {
        res = connections_to_clients[i].SetupConnection(i);
        if (!res) {
            return false;
        }
    }
    std::cout << "Host with pid: " << getpid() << std::endl;
    return true;
}


void Wolf::SetGoatsNumber(int n) {
    goats_number = n;
    delete[] connections_to_clients;
    delete[] clients_threads;
    connections_to_clients = new ClientHostConnection[n];
    clients_threads = new pthread_t[n];
}

int Wolf::GenerateClientId() {
    static int i = 0;
    return i < goats_number ? i++ : -1;
}

void Wolf::GenerateNumber() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned> distribution(1, 100);
    current_answer = distribution(gen);
}


void Wolf::Terminate(int signum) {
    for (int i = 0; i < goats_number; ++i) {
        kill(connections_to_clients[i].GetClientPid(), signum);
        pthread_cancel(clients_threads[i]);
    }
    is_running = false;
}

Wolf::~Wolf() {
    for (int i = 0; i < goats_number; i++)
        connections_to_clients[i].Delete();
    delete[] connections_to_clients;
    delete[] clients_threads;

    pthread_cond_destroy(&cond_var);
    pthread_mutex_destroy(&mutex);
}

int Wolf::Decide(int goat_answer, ClientHostConnection &info) {
    if (!info.GetDaysDead()) {
        if (abs(current_answer - goat_answer) <= 70 / goats_number) {
            info.SetAlive();
            return 1;
        } else {
            info.IncrDaysDead();
            return 0;
        }
    } else {
        if (abs(current_answer - goat_answer) <= floor(20 / goats_number)) {
            info.SetAlive();
            return 1;
        } else {
            info.IncrDaysDead();
            return 0;
        }
    }
}


void Wolf::Start() {
    if (!goats_number)
        return;
    int goatsResponse = 0;
    while (goatsResponse < goats_number) {
        goatsResponse = 0;
        for (int i = 0; i < goats_number; ++i) {
            if (connections_to_clients[i].HasSignalledBack())
                goatsResponse++;
        }
    }

    for (int i = 0; i < goats_number; ++i) {
        pthread_create(&(clients_threads[i]), nullptr, Thread, &connections_to_clients[i]);
    }

    while (is_running) {
        if (goats_answered == goats_number) {
            pthread_mutex_lock(&mutex);
            int goats_alive = 0;
            for (int i = 0; i < goats_number; ++i) {
                if (connections_to_clients[i].GetDaysDead() < 2)
                    ++goats_alive;
            }

            if (!goats_alive) {
                syslog(LOG_INFO, "All goats have been dead for 2 steps in a row");
                Terminate(SIGINT);
            }
            GenerateNumber();
            syslog(LOG_NOTICE, "Wolf has picked number %i", current_answer.load());
            goats_answered = 0;
            current_step++;
            pthread_cond_broadcast(&cond_var);
            pthread_mutex_unlock(&mutex);
        } else {
            sleep(1);
        }
    }
}

void Wolf::SignalHandler(int signum, siginfo_t* info, void* ptr) {
    Wolf& wolfInst = GetWolfInst();
    switch (signum) {
        case SIGUSR1: {
            int clientId = wolfInst.GenerateClientId();
            if (clientId < 0) {
                syslog(LOG_ERR, "Client list is full");
            } else {
                union sigval value;
                value.sival_int = clientId;
                if (!sigqueue(info->si_pid, SIGUSR1, value)) {
                    wolfInst.connections_to_clients[clientId].OnSignalBack(info->si_pid);
                } else {
                    syslog(LOG_ERR, "%s", strerror(errno));
                }
            }
            break;
        }
        case SIGUSR2: {
            wolfInst.Terminate(SIGINT);
        }
        default:
            wolfInst.Terminate(signum);
    }
}

void* Wolf::Thread (void* arg) {
    auto* info = (ClientHostConnection*) arg;
    Wolf &instance = Wolf::GetWolfInst();
    sem_t& semaphore = info->GetSemaphore();
    sem_t& semaphore_host = info->GetHostSemaphore();
    Conn connection = info->GetConnection();

    int clientStep = 0;
    struct timespec ts;
    int res;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    instance.goats_answered++;
    while (true) {
        if (info->HasSignalledBack()) {
            pthread_mutex_lock(&instance.mutex);
            while (clientStep >= instance.current_step)
                pthread_cond_wait(&instance.cond_var, &instance.mutex);
            clientStep++;

            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 5;
            if (sem_timedwait(&semaphore_host, &ts) == -1) {
                syslog(LOG_NOTICE, "Wait time has expired");
                instance.Terminate(SIGTERM);
            }
            if (connection.Read(&res, sizeof(int))) {
                res = instance.Decide(res, *info);
                if (!connection.Write(&res, sizeof(int))) {
                    syslog(LOG_ERR, "Wolf Could not write");
                }
            } else {
                syslog(LOG_NOTICE, "Wolf Could not read");
            }
            sem_post(&semaphore);
            instance.goats_answered++;
            pthread_mutex_unlock(&instance.mutex);
        } else {
            pthread_exit(nullptr);
        }
    }
}
