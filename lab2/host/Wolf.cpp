#include "Wolf.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <syslog.h>
#include <vector>


Wolf* Wolf::GetInstance() {
    static Wolf self;
    return &self;
}

void Wolf::SetClientsCount(int num) {
    _numOfClients = num;
    _valAlive = 70 / _numOfClients;
    _valDeath = 20 / _numOfClients;
    _deadTurns = 0;
    _hostConnections.clear();
    _hostConnections = std::vector<Host_conn*>();
    _clientsHandlers.clear();
    _clientsHandlers = std::vector<Client_conn*>();
}

Wolf::Wolf() {
    struct sigaction act{};
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = SignalHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
}

void Wolf::CreateConnection(Client_conn* handler) {
    int client_pid;
    client_pid = fork();
    if (client_pid == 0) {
        Goat* client = Goat::GetInstance(handler->GetID());
        client->set_conn(handler->GetConn());
        if(!client->CreateConnection()) {
            syslog(LOG_ERR, "Impossible to connect handler and client");
            return;
        }
        client->StartRand();
        syslog(LOG_INFO, "Client created");
        return;
    }
    handler->SetClient(client_pid);
}

void* Wolf::RunClient(void *arg) {
    auto* handler = (Client_conn*)arg;
    handler->Start();
    return nullptr;
}

void Wolf::Start() {
    syslog(LOG_INFO, "Creation of all clients started in Wolf");
    auto *conn = new Host_conn();
    for (int i = 0; i < _numOfClients; i++) {
        auto *handler = new Client_conn(i);
        conn->CreateConnect(handler->GetID());
        handler->SetupServerConnection(conn);
        if(!handler->OpenConnection()) {
            syslog(LOG_ERR, "Error in _connection establishment: %s", strerror(errno));
            return;
        }
        CreateConnection(handler);
        _clientsHandlers.push_back(handler);
        _hostConnections.push_back(conn);

        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        handler->SetTID(tid);
        void *(*function)(void *) = RunClient;
        pthread_create(&tid, &attr, function, handler);
    }
    RunGame();
}

void Wolf::SignalHandler(int signum, siginfo_t *info, void *ptr) {
    static Wolf* instance = GetInstance();
    switch (signum)
    {
        case SIGTERM:
        {
            instance->Terminate(signum);
            break;
        }
    }
}

void Wolf::Terminate(int signum) {
    syslog(LOG_INFO, "Starting Wolf's disconnection");
    for (int i = 0; i < _numOfClients; i++) {
        _hostConnections[i]->CloseConnect();
    }
    closelog();
    exit(signum);
}


void Wolf::RunGame() {
    int value = 0;
    SendAliveStatus(1);
    while (true) {
        std::cout << "if you want to exit, please enter -100" << std::endl;
        std::cout << "Enter wolf val -> ";
        std::cin >> value;
        
        if (value == -100 || _deadTurns >= 2) {
            syslog(LOG_INFO, "Terminating Wolf...");
            Terminate(EXIT_SUCCESS);
        }
        WolfWork(value);
    }
}

void Wolf::SendAliveStatus(int msg) {
    for (int i = 0; i < _numOfClients; i++) {
        if (!_hostConnections[i]->IsClosed()) {
            _hostConnections[i]->Write(&msg, sizeof(msg));
            _hostConnections[i]->SetStat(true);
            _hostConnections[i]->SignTo();
        }
    }
}


void Wolf::WolfWork(int msg) {
    int alive = 1;
    int dead = 0;
    int valOfDead = 0;
    int i = 0;
    int numOfAnswers = 0;
    std::vector<int> answers(_numOfClients);
    while(numOfAnswers < _numOfClients) {
        if(answers[i] == 0) {
            if(_hostConnections[i]->IsClientReady()) {
                if (_hostConnections[i]->GetSat()) {
                    int num;
                    _hostConnections[i]->Read(&num, sizeof(int));
                    if (std::abs(num - msg) < _valAlive) {
                        _hostConnections[i]->Write(&alive, sizeof(int));
                    } else {
                        _hostConnections[i]->Write(&dead, sizeof(int));
                        valOfDead++;
                    }
                } else {
                    int num;
                    _hostConnections[i]->Read(&num, sizeof(int));
                    if (std::abs(num - msg) < _valDeath) {
                        _hostConnections[i]->Write(&alive, sizeof(int));
                    } else {
                        _hostConnections[i]->Write(&dead, sizeof(int));
                        valOfDead++;
                    }
                }
                numOfAnswers++;
                answers[i]++;
            }
        }
        i++;
        if(i ==_numOfClients){
            if(numOfAnswers != _numOfClients){
                i = 0;
            }
        }
    }
    if(valOfDead == _numOfClients){
        _deadTurns++;
    }else{
        _deadTurns = 0;
    }
}

Wolf::Wolf(Wolf &s) {}
