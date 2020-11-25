#include <iostream>
#include <sys/un.h>
#include <sys/socket.h>
#include <syslog.h>
#include <unistd.h>

#include "conn.h"


struct SockConnData {
    int _hsocket;
    int _csocket;
};


Conn::Conn(int host_pid_, bool create) {
    openlog("GameConnection", LOG_PID, LOG_DAEMON);
    _owner = create;
    _hostPid = host_pid_;
    std::string socketpath = std::string("/tmp/" + std::to_string(_hostPid));
    SockConnData socketdata;

    struct sockaddr_un serv_addr;
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, socketpath.c_str());
    syslog(LOG_INFO, "Socket path is %s", socketpath.c_str());
    std::cout << "Socket path is " << socketpath << std::endl;

    if (create) {
        syslog(LOG_INFO, "Create listener");
        std::cout << "Create listener" << std::endl;
        socketdata._hsocket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (socketdata._hsocket == -1) {
            throw std::runtime_error("hsocket error");
        }

        syslog(LOG_INFO, "Bind listener");
        std::cout << "Bind listener" << std::endl;
        if (bind(socketdata._hsocket, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
            throw std::runtime_error("bind error");
        }

        syslog(LOG_INFO, "Listen");
        std::cout << "Listen" << std::endl;
        if (listen(socketdata._hsocket, 1) < 0 ) {
            throw std::runtime_error("listen error");
        }

        syslog(LOG_INFO, "Accept");
        std::cout << "Accept" << std::endl;
        socketdata._csocket = accept(socketdata._hsocket, NULL, NULL);
        if (socketdata._csocket < 0) {
            throw std::runtime_error("accept error");
        }
    } else {
        socketdata._csocket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (socketdata._csocket < 0) {
            throw std::runtime_error("csocket error");
        }
        syslog(LOG_INFO, "Socket created");
        std::cout << "Socket created" << std::endl;

        if (connect(socketdata._csocket,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            throw std::runtime_error("connect error");
        }
        syslog(LOG_INFO, "Socket connected");
        std::cout << "Socket connected" << std::endl;
    }

    _pData = new (std::nothrow) SockConnData(socketdata);
}


Conn::~Conn() {
    SockConnData* socketdata = (SockConnData*)_pData;
    std::string socketpath = std::string("/tmp/" + std::to_string(_hostPid));

    if (!_owner) {
        if (socketdata->_csocket != -1) {
            if (close(socketdata->_csocket) < 0) {
                perror("close(csocket) ");
            }
        }
    }

    if (_owner) {
        if (socketdata->_hsocket != -1) {
            if (close(socketdata->_hsocket) < 0) {
                perror("close(hsocket) ");
            }
        }

        unlink(socketpath.c_str());
    }

    delete (SockConnData*)_pData;

    syslog(LOG_INFO, "Connection closed");
    std::cout << "Connection closed" << std::endl;
    closelog();
}


bool Conn::Read(void* buf, size_t count) noexcept {
    SockConnData* socketdata = (SockConnData*)_pData;
    int n = recv(socketdata->_csocket, buf, count, 0);
    if (n < 0) {
        perror("read()");
        return false;
    }
    return true;
}


bool Conn::Write(void* buf, size_t count) noexcept {
    SockConnData* socketdata = (SockConnData*)_pData;
    int n = send(socketdata->_csocket, buf, count, MSG_NOSIGNAL);
    if (n < 0) {
        perror("write() ");
        return false;
    }
    return true;
}
