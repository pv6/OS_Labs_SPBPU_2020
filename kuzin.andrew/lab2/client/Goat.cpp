#include "Goat.h"

Goat& Goat::getInstance()
{
    static Goat inst;
    return inst;
}

Goat::Goat()
{
    hostPid_ = 0;
    id_ = -1;
    semaphoreHost_ = nullptr;
    semaphoreClient_ = nullptr;

    struct sigaction sigAct;
    sigAct.sa_sigaction = signalHandler;
    sigAct.sa_flags = SA_SIGINFO | SA_RESTART;
    sigaction(SIGTERM, &sigAct, nullptr);
    sigaction(SIGINT, &sigAct, nullptr);
    sigaction(SIGUSR1, &sigAct, nullptr);
    sigaction(SIGALRM, &sigAct, nullptr);
}

void Goat::startWork()
{
    Message msg;

    msg.number_ = getRand(Status::ALIVE);

    std::cout << "------------ Goat id " << id_ << " ----------------" <<std::endl;
    std::cout << "Goat number: " << msg.number_ << std::endl;
    connection_.connSend(&msg, sizeof(msg));
    sem_post(semaphoreHost_);

    while (true) {
        sem_wait(semaphoreClient_);
        if (connection_.connReceive(&msg, sizeof(Message))) {
            std::cout << "Wolf number: " << msg.number_ << std::endl;
            std::cout << "Status: " << ((msg.status_ == Status::ALIVE) ? "alive" : "dead") << std::endl;

            msg.number_ = getRand(msg.status_);

            std::cout << "------------ Goat id " << id_ << " ----------------" <<std::endl;
            std::cout << "Goat number: " << msg.number_ << std::endl;
            connection_.connSend(&msg, sizeof(msg));
        }
        sem_post(semaphoreHost_);
    }
}

void Goat::terminate(int sig)
{
    kill(hostPid_, SIGUSR2);
    if (sem_close(semaphoreClient_) == -1 || sem_close(semaphoreHost_) == -1) {
        std::cout << "Failed: " << strerror(errno) << std::endl;
        exit(errno);
    }
    if (!connection_.connClose()) {
        std::cout << "Failed: " << strerror(errno) << std::endl;
        exit(errno);
    }
    std::cout << "Exit" << std::endl;
    exit(sig);
}

int Goat::getRand(Status status) {
    std::random_device r;
    std::default_random_engine dre(r());
    std::uniform_int_distribution<int> dist(1, ((status == Status::ALIVE) ? LIMIT_ALIVE : LIMIT_DEAD));
    return dist(dre);
}

void Goat::signalHandler(int sig, siginfo_t* info, void* ptr)
{
    Goat& inst = Goat::getInstance();
    switch (sig) {
        case SIGUSR1: {
            inst.id_ = info->si_value.sival_int;
            break;
        }
        case SIGALRM: {
            if (inst.id_ >= 0)
                break;
        }
        default: {
            inst.terminate(sig);
        }
    }
}

bool Goat::openConnection()
{
    bool res = false;

    if (hostPid_ == 0)
        return false;

    alarm(TIMEOUT);
    kill(hostPid_, SIGUSR1);

    while (id_ < 0)
        pause();

    if (connection_.connOpen(id_, false)) {
        semaphoreHost_ = sem_open((SEMAPHORE_HOST_NAME + std::to_string(id_)).c_str(), 0);
        semaphoreClient_ = sem_open((SEMAPHORE_CLIENT_NAME + std::to_string(id_)).c_str(), 0);
        if (semaphoreHost_ == SEM_FAILED || semaphoreClient_ == SEM_FAILED) {
            std::cout << "ERROR: sem_open failed: " << strerror(errno) << std::endl;
        } else {
            res = true;
            std::cout << "pid of created client is: " << getpid() << std::endl;
        }
    }
    return res;
}

void Goat::run(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "ERROR: wrong args - expected host pid" << std::endl;
        throw std::exception();
    }

    int pid;
    pid = std::stoi(argv[1]);

    std::cout << "Open connection..." << std::endl;
    hostPid_ = pid;

    if (openConnection()) {
        std::cout << "Starting client..." << std::endl;
        std::cout << std::endl;
        startWork();
    }
}