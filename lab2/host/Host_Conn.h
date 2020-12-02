//
// Created by yudzhinnsk on 27.11.2020.
//

#ifndef LASTTRY_HOST_CONN_H
#define LASTTRY_HOST_CONN_H

#include <pthread.h>


class Host_conn {
public:
    bool CreateConnect(int id);
    bool CloseConnect();
    bool Read(void *buf, size_t size);
    bool Write(void *buf, size_t size);
    bool IsClosed() const { return _isClosed; };
    void SignTo();
    bool HasSign() const;
    void SetReady(bool val);
    bool IsClientReady();
    void SetStat(bool stat);
    bool GetSat();
private:
    bool _signTo = false;
    int _idClient;
    bool _isClosed = true;
    bool _isAlive = true;
    int _fd[2];

    bool _isClientReady = false;

};
#endif //LASTTRY_HOST_CONN_H
