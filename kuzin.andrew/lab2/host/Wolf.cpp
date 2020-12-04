#include "Wolf.h"

Wolf& Wolf::getInstance()
{
    static Wolf inst;
    return inst;
}

Wolf::Wolf()
{
    step_ = 0;
    stepNumber_ = 0;
    finishAmount_ = 0;
    clientsAmount_ = 0;
    clientInfo_ = nullptr;
    threads_ = nullptr;
    attr_ = nullptr;

    struct sigaction sigAct;
    sigAct.sa_sigaction = signalHandler;
    sigAct.sa_flags = SA_SIGINFO | SA_RESTART;
    sigaction(SIGTERM, &sigAct, nullptr);
    sigaction(SIGINT, &sigAct, nullptr);
    sigaction(SIGUSR1, &sigAct, nullptr);
    sigaction(SIGUSR2, &sigAct, nullptr);

    pthread_cond_init(&cond1_, nullptr);
    pthread_cond_init(&cond2_, nullptr);
    pthread_mutex_init(&mutex_, nullptr);
}

Wolf::~Wolf()
{
    for (int i = 0; i < clientsAmount_; i++)
        clientInfo_[i].free();

    delete[] threads_;
    delete[] attr_;
    delete[] clientInfo_;

    pthread_cond_destroy(&cond1_);
    pthread_cond_destroy(&cond2_);
    pthread_mutex_destroy(&mutex_);
}

Message Wolf::step(Message& ans, GoatInformation& info) {
    Message msg;
    Wolf &inst = getInstance();
    msg.number_ = inst.stepNumber_;
    if ((ans.status_ == Status::ALIVE && abs(msg.number_ - ans.number_) <= 70 / inst.clientsAmount_) ||
        (ans.status_ == Status::DEAD && abs(msg.number_ - ans.number_) <= 20 / inst.clientsAmount_)) {
        info.countStepsDead_ = 0;
    } else {
        msg.status_ = Status::DEAD;
        info.countStepsDead_++;
    }
    return msg;
}

void Wolf::getNumber() {

    std::cout << std::endl << "Input number (in range [1," << LIMIT << "]): " << std::endl;

    int num = -1;
    while (num == -1) {
        std::cin >> num;

        if (!(std::cin.fail())) {
            if (num >= 1 && num <= LIMIT)
                stepNumber_ = num;
            else {
                std::cout << "Number should be in range [1, " << LIMIT << "]. Try again!" << std::endl;
                num = -1;
            }
        } else {
            num = -1;
            std::cout << "Input should be a number in range [1, " << LIMIT << "]. Try again!" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

int Wolf::getClientId()
{
    static int i = 0;
    return (i != clientsAmount_) ? i++ : -1;
}

void Wolf::startWork() {
    int attachedClients = 0;

    while (attachedClients != clientsAmount_) {
        attachedClients = 0;
        for (int i = 0; i < clientsAmount_; ++i) {
            if (clientInfo_[i].isAttached())
                attachedClients++;
        }

        if (attachedClients != clientsAmount_) {
            std::cout << std::endl << "Waiting " << clientsAmount_ - attachedClients << " clients" << std::endl;
            pause();
        }
    }
    std::cout << "Clients attached!" << std::endl;

    for (int i = 0; i < clientsAmount_; ++i) {
        pthread_attr_init(&attr_[i]);
        pthread_create(&(threads_[i]), &attr_[i], threadRun, &clientInfo_[i]);
    }
    while (true) {
        pthread_mutex_lock(&mutex_);
        getNumber();
        step_++;
        pthread_cond_broadcast(&cond1_);
        while (finishAmount_ != clientsAmount_) {
            pthread_cond_wait(&cond2_, &mutex_);
        }
        finishAmount_ = 0;
        pthread_mutex_unlock(&mutex_);

        int res = 0;
        for (int i = 0; i < clientsAmount_; ++i) {
            if (clientInfo_[i].countStepsDead_ < 2)
                ++res;
        }

        if (res == 0) {
            std::cout << std::endl << "GAME OVER - All goats are dead during 2 steps" << std::endl;
            terminate(SIGINT);
        }
    }
}

void Wolf::setClientsAmount(int n) {
    clientsAmount_ = n;
    clientInfo_ = new GoatInformation[n];
    threads_ = new pthread_t[n];
    attr_ = new pthread_attr_t[n];
}

bool Wolf::openConnection() const
{
    bool res = true;

    for (int i = 0; i < clientsAmount_; ++i)
        res &= clientInfo_[i].openConnection(i);

    std::cout << std::endl;
    if (res)
        std::cout << "Created host pid: " << getpid() << std::endl;
    else
        std::cout << "Open connection error" << std::endl;

    return res;
}

void Wolf::terminate(int sig) {
    for (int i = 0; i < clientsAmount_; ++i)
        clientInfo_[i].detach();

    for (int i = 0; i < clientsAmount_; ++i)
        kill(clientInfo_[i].getPid(), sig);

    for (int i = 0; i < clientsAmount_; i++)
        pthread_cancel(threads_[i]);

    std::cout << "Exit" << std::endl;
    exit(sig);
}

void Wolf::signalHandler(int sig, siginfo_t* info, void* ptr) {
    Wolf &inst = getInstance();
    switch (sig) {
        case SIGUSR1: {
            int id = inst.getClientId();
            if (id != -1) {
                __sigval_t value;
                value.sival_int = id;
                if (sigqueue(info->si_pid, SIGUSR1, value) == 0) {
                    std::cout << std::endl << "Attaching client - pid: " << info->si_pid << ", id: " << id << std::endl;
                    inst.clientInfo_[id].attach(info->si_pid);
                }

            } else
                std::cout << "Too many clients" << std::endl;
            break;
        }
        case SIGUSR2: {
            for (int i = 0; i < inst.clientsAmount_; ++i)
                if (inst.clientInfo_[i].getPid() == info->si_pid && inst.clientInfo_[i].isAttached()) {
                    inst.terminate(SIGINT);
                    break;
                }
            break;
        }
        default: {
            if (getpid() != info->si_pid) {
                inst.terminate(sig);
            }
        }
    }
}

void* Wolf::threadRun(void* data) {
    GoatInformation* goatInfo = (GoatInformation*) data;
    Wolf& inst = Wolf::getInstance();
    int id = goatInfo->getId();
    sem_t &semaphore_host = goatInfo->getHostSemaphore();
    sem_t &semaphore_client = goatInfo->getClientSemaphore();
    Conn connection = goatInfo->getConnection();

    int clientStep = 0;
    struct timespec ts;
    Message msg;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    while (true) {
        if (goatInfo->isAttached()) {
            pthread_mutex_lock(&inst.mutex_);
            while (clientStep >= inst.step_)
                pthread_cond_wait(&inst.cond1_, &inst.mutex_);
            clientStep++;

            ++inst.finishAmount_;

            if (inst.finishAmount_ == inst.clientsAmount_) {
                pthread_cond_signal(&inst.cond2_);
            }
            pthread_mutex_unlock(&inst.mutex_);

            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += TIMEOUT;
            if (sem_timedwait(&semaphore_host, &ts) == -1) {
                std::cout << "Time wait id: " << id << std::endl;
                inst.terminate(SIGTERM);
            }
            if (connection.connReceive(&msg, sizeof(msg))) {
                std::cout << "--------------- Goat id " << id << " -----------------" << std::endl;
                std::cout << "Goat current status: " << ((msg.status_ == Status::ALIVE) ? "alive" : "dead") << std::endl;
                std::cout << "Goat number: " << msg.number_ << std::endl;

                msg = inst.step(msg, *goatInfo);

                std::cout << "Goat new status: " << ((msg.status_ == Status::ALIVE) ? "alive" : "dead") << std::endl;

                connection.connSend(&msg, sizeof(msg));

            }

            sem_post(&semaphore_client);

        } else
            pthread_exit(nullptr);
    }
}

void Wolf::run(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "ERROR: wrong args - expected number of goats" << std::endl;
        throw std::exception();
    }

    int amount = std::stoi(argv[1]);
    if (amount <= 0) {
        std::cout << "Number of goats must be > 0";
        throw std::exception();
    }

    setClientsAmount(amount);

    std::cout << "Open connection..." << std::endl;
    std::cout << "pid: " << getpid() << std::endl;


    if (openConnection()) {
        std::cout << "Starting host..." << std::endl;
        std::cout << std::endl;
        startWork();
    }
}