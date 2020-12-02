//
// Created by yudzhinnsk on 27.11.2020.
//

#ifndef LASTTRY_WOLF_H
#define LASTTRY_WOLF_H

#include <signal.h>
#include <vector>
#include "Host_Conn.h"
#include "Client_conn.h"

class Wolf {
public:
    static Wolf* GetInstance();
    void Start();
    void SetClientsCount(int num);
private:
    Wolf &operator=(Wolf& s) {
        return s;
    }
    int _numOfClients = 0;
    std::vector<Host_conn*> _hostConnections;
    std::vector<Client_conn*> _clientsHandlers;
    Wolf();
    Wolf(Wolf& s);
    void CreateConnection(Client_conn* handler);
    void SendAliveStatus(int msg);
    static void* RunClient(void* arg);
    static void SignalHandler(int signum, siginfo_t *info, void *ptr);
    void Terminate(int signum);
    void RunGame();

    void WolfWork(int msg);

    int _valDeath;
    int _valAlive;
    int _deadTurns;
};
#endif //LASTTRY_WOLF_H
