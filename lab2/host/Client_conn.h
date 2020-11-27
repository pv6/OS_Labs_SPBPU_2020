//
// Created by yudzhinnsk on 27.11.2020.
//

#ifndef LASTTRY_CLIENT_CONN_H
#define LASTTRY_CLIENT_CONN_H

#include "Goat.h"
#include "Host_Conn.h"
#include <semaphore.h>
#include <string>

class Client_conn {
public:
    Client_conn() = default;
    Client_conn(int id);
    void SetClient(int pid);
    void Start();
    void SetupServerConnection(Host_conn* connection);
    bool OpenConnection();
    void SetTID(pthread_t tid);
    pthread_t get_tid();
    int GetID();
    Conn GetConn();
    Client_conn(Client_conn& c) = default;
    Client_conn& operator=(Client_conn& c);
    ~Client_conn() {
        Terminate(EXIT_SUCCESS);
    }
private:
    int _clientId;
    int _clientPid;
    bool _isConnected;
    bool _isClosed;
    Host_conn* _hostConn;
    Conn _conn;
    sem_t* semaphore_host;
    sem_t* semaphore_client;

    std::string sem_client_name;
    std::string sem_host_name;

    pthread_t _tid;

    bool GetAliveStat(int &val);

    void KillClient();

    void Terminate(int signum);

    const int timeout = 5;
};
#endif //LASTTRY_CLIENT_CONN_H
