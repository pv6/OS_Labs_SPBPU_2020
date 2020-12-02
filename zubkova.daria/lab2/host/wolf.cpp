#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include "wolf.h"
#include "goat.h"
#include <fcntl.h>
#include <csignal>
#include "../conn/conn.h"
#include <semaphore.h>
#include <random>

#define TIMEOUT 5

std::list<str*> Wolf::structClients = std::list<str*>();
std::map<int, Message> Wolf::clientsMessages = std::map<int, Message>();
int Wolf::numWolf = 0;
int Wolf::numGoats = 0;

Wolf::Wolf(){
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = SignalHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
}

Wolf::Wolf(Wolf& other){}

Wolf& Wolf::operator=(Wolf& other) {
    return other;
}

Wolf::~Wolf() = default;

Wolf* Wolf::GetInstance() {
    static Wolf instance;
    return &instance;
}

void Wolf::Terminate(int signum){
    for (auto client : structClients){
        kill(client->pid, SIGTERM);
        sem_post(client->semClient);
        if (client->con != nullptr) {
            if (!client->con->Close()) {
                syslog(LOG_ERR, "ERROR: %s", strerror(errno));
            }
            client->con = nullptr;
        }
        if (client->semClient != nullptr && client->semHost != nullptr) {
            std::string semName = "sem_client_" + std::to_string(client->id);
            std::string semName2 = "sem_host_" + std::to_string(client->id);
            if (sem_unlink(semName.c_str()) == -1 || sem_unlink(semName2.c_str()) == -1) {
                syslog(LOG_ERR, "ERROR: %s", strerror(errno));
            }
            client->semClient = nullptr;
            client->semHost = nullptr;
        }
    }
    while (!structClients.empty()) {
        delete structClients.front();
        structClients.pop_front();
    }
    clientsMessages.clear();
}

void* Wolf::ReadGoats(void* param) {
    syslog(LOG_NOTICE, "Run read wolf");
    //Goat* client = (Goat*) param;
    str* client = (str*)param;
    Message buf(Owner::WOLF);
    if (client->con->Read(&buf, sizeof(buf))) {
        if (buf.owner != Owner::WOLF) {
            //std::cout << "Goat Num " << buf.num << std::endl;
            clientsMessages[client->id] = buf;
            return nullptr;
        } else {
            syslog(LOG_NOTICE, "Wolf write again");
            if (!client->con->Write(&buf, sizeof(buf))){
                syslog(LOG_ERR, "ERROR: Can't write in wolf");
                return nullptr;
            }
        }
    } else {
        syslog(LOG_NOTICE, "ERROR: Can't read in wolf");
    }
    return nullptr;
}

void* Wolf::WriteGoats(void* param) {
    syslog(LOG_NOTICE, "Run write wolf");
    struct timespec ts;
    str* client = (str*) param;
    //sem_t* semHost = client->GetSemHost();
    //sem_t* semClient = client->GetSemClient();
    int clientNumber = clientsMessages[client->id].num;
    Status st = clientsMessages[client->id].st;
    if (st == Status::ALIVE && abs(numWolf - clientNumber) <= 70 / numGoats) {
        st = Status::ALIVE;
        clientsMessages[client->id].st = Status::ALIVE;
    } else if (st == Status::DEAD && abs(numWolf - clientNumber) <= 20 / numGoats) {
        st = Status::ALIVE;
        clientsMessages[client->id].st = Status::ALIVE;
    } else {
        st = Status::DEAD;
        clientsMessages[client->id].st = Status::DEAD;
    }
    Message msg(Owner::WOLF, st, numWolf);
    if (!client->con->Write(&msg, sizeof(msg))){
        syslog(LOG_ERR, "ERROR: Can't write in wolf");
        return nullptr;
    }
    sem_post(client->semClient);
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += TIMEOUT;
    if (sem_timedwait(client->semHost, &ts) == -1) {
        syslog(LOG_NOTICE, "ERROR: Wait time finish");
        KillClient(client);
        return nullptr;
    }
    return nullptr;
}

void Wolf::KillClient(str* client){
    kill(client->pid, SIGTERM);
    structClients.remove(client);
    numGoats--;
}

void Wolf::Threads(void* (*function) (void*)){
    std::list <pthread_t> tids;
    for (auto client : structClients) {
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, function, client);
        tids.push_back(tid);
    }
    for (auto tid : tids) {
        pthread_join(tid, nullptr);
    }
}

void Wolf::Start() {
    if (CreateGoats()) {
        Process();
    }
}

bool Wolf::CreateGoats() {
    std::cout << "Enter number of goats: ";
    struct timespec ts;
    while (true) {
        std::cin >> numGoats;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(256, '\n');
        } else if (numGoats > 0)
            break;
        std::cout << "ERROR: Wrong input. Enter number of goats > 0." << std::endl;
    }
    for (int i = 0; i < numGoats; i++) {
        conn* curConnection = new conn();
        if (!curConnection->Open(i, true)) {
            //std::cout << "ERROR: Can`t open goat(" << i << ") =  " << strerror(errno)<< std::endl;
            syslog(LOG_ERR, "ERROR: Can`t open goat( << %i << ) = %s", i, strerror(errno));
            continue;
        }

        std::string sem_client_name = "sem_client_" + std::to_string(i);
        std::string sem_host_name = "sem_host_" + std::to_string(i);

        sem_t* semHost = sem_open(sem_host_name.c_str(), O_CREAT, 0666, 0);
        if (semHost == SEM_FAILED) {
            //std::cout << "ERROR: can`t open host semaphore = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: can`t open host semaphore = %s", strerror(errno));
            continue;
        }
        //std::cout << "Host semaphore create (" << sem_host_name << ")" << std::endl;
        syslog(LOG_NOTICE, "Host semaphore create (%s)", sem_host_name.c_str());

        sem_t* semClient = sem_open(sem_client_name.c_str(), O_CREAT, 0666, 0);
        if (semClient == SEM_FAILED) {
            //std::cout << "ERROR: can`t open client semaphore = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: can`t open client semaphore = %s", strerror(errno));
            sem_unlink(sem_host_name.c_str());
            continue;
        }
        //std::cout << "Client semaphore create (" << sem_client_name << ")" << std::endl;
        syslog(LOG_NOTICE, "Client semaphore create (%s)", sem_client_name.c_str());
        //std::cout << "Client numer: " << i << std::endl;
        syslog(LOG_NOTICE, "Clinet numer: %d", i);

        Message msg(Owner::WOLF);
        if (!curConnection->Write(&msg, sizeof(msg))){
            //std::cout << "ERROR: Can`t write goat(" << i << ") = " << strerror(errno)<< std::endl;
            syslog(LOG_ERR, "ERROR: Can`t write goat( << %i << ) = %s", i, strerror(errno));
            if (semClient != nullptr && semHost != nullptr) {
                std::string semName = "sem_client_" + std::to_string(i);
                std::string semName2 = "sem_host_" + std::to_string(i);
                if (sem_unlink(semName.c_str()) == -1 || sem_unlink(semName2.c_str()) == -1) {
                    syslog(LOG_ERR, "ERROR: %s", strerror(errno));
                }
            }
            continue;
        }

        str* s = new str;
        s->id = i;
        s->con = curConnection;
        s->semHost = semHost;
        s->semClient = semClient;

        int pid = fork();
        //std::cout << " PID: " << pid << std::endl;
        s->pid = pid;

        if (pid == 0) {
            Goat* client = Goat::GetInstance(pid);
            client->Set(curConnection, semHost, semClient, i);
            client->Start();
            return false;
        }
        sem_post(semClient);
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += TIMEOUT;
        if (sem_timedwait(semHost, &ts) == -1) {
            syslog(LOG_NOTICE, "ERROR: Wait time finish");
            KillClient(s);
            continue;
        }
        structClients.push_back(s);
    }
    return true;
}


void Wolf::Process() {
    std::cout << "Game:" << std::endl;
    syslog(LOG_NOTICE, "Game:");
    int dead = 0;
    int numDead = 0;
    while (st) {

       Threads(Wolf::ReadGoats);

        for (auto client : structClients) {
            std::string goatSt = clientsMessages[client->id].st == Status::ALIVE ? "ALIVE" : "DEAD";
            std::cout << "Goat's number: " << clientsMessages[client->id].num << " & status: " << goatSt << std::endl;
            syslog(LOG_NOTICE, "Goat's number: %i & status: %s", clientsMessages[client->id].num, goatSt.c_str());
        }

        std::random_device rd;
        std::minstd_rand mt(rd());
        std::uniform_int_distribution<int> dist(1, 100);
        numWolf = dist(mt);
        std::cout << "Wolf's number: " << numWolf << std::endl;
        syslog(LOG_NOTICE, "Wolf's number: %i", numWolf);

        Threads(Wolf::WriteGoats);

        numDead = 0;
        for (auto client : structClients) {
            if (clientsMessages[client->id].st == Status::DEAD)
                numDead++;
        }
        std::cout << "Number of dead: " << numDead << std::endl;
        syslog(LOG_NOTICE, "Number of dead: %i", numDead);
        if (numDead == numGoats)
            dead++;
        else
            dead = 0;
        if (dead == 2) {
            std::cout << "Wolf is winner" << std::endl;
            syslog(LOG_NOTICE, "Wolf is winner");
            st = false;
            continue;
        }
    }
    Terminate(SIGTERM);
}

void Wolf::SignalHandler(int signum, siginfo_t* info, void* ptr){
    static Wolf* instance = GetInstance();
    switch (signum)
    {
        case SIGTERM:
        {
            instance->st = false;
            instance->Terminate(signum);
            break;
        }
    }
}
