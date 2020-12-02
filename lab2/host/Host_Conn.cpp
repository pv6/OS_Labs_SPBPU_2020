//
// Created by yudzhinnsk on 11/25/2020.
//
#include "Host_Conn.h"
#include <string>
#include <syslog.h>
#include <unistd.h>
#include <cstring>



bool Host_conn::CreateConnect(int id)  {
    _isClosed = false;
    _signTo = false;
    _idClient = id;
    if (pipe(_fd) == -1) {
        syslog(LOG_ERR, "Pipe creation failed: %s", strerror(errno));
        return false;
    }
    return true;
}

bool Host_conn::CloseConnect() {
    if (close(_fd[0]) < 0 && close(_fd[1]) < 0) {
        syslog(LOG_ERR, "Disconnection of pipe failed: %s", strerror(errno));
        return false;
    }
    _isClosed = true;
    return true;
}

bool Host_conn::Read(void *buf, size_t size) {
    if (::read(_fd[0], buf, size) <= 0) {
        syslog(LOG_ERR, "Reading failed with error = %s", strerror(errno));
        return false;
    }
    return true;
}


bool Host_conn::Write(void *buf, size_t size) {
    if (::write(_fd[1], buf, size) == -1) {
        syslog(LOG_ERR, "Writing failed with error = %s", strerror(errno));
        return false;
    }
    return true;
}

void Host_conn::SignTo() {
    this->_signTo = true;
}

bool Host_conn::HasSign() const{
    return _signTo;
}

void Host_conn::SetStat(bool stat) {
    _isAlive = stat;
}

bool Host_conn::GetSat() {
    return _isAlive;
}


void Host_conn::SetReady(bool val){
    this->_isClientReady = val;
}

bool Host_conn::IsClientReady() {
    return _isClientReady;
}

