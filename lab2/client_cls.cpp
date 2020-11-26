#include "client_cls.h"

Client &Client::getInstance(int host_pid) {
    static Client instance(host_pid);
    return instance;
}

Client::Client(int host_pid){
    this->host_pid = host_pid;
    signal(SIGTERM, handleSignal);
    signal(SIGINT, handleSignal);
}

void Client::handleSignal(int signum) {
    Client &client = getInstance(0);
    switch (signum)
    {
    case SIGTERM:
    case SIGINT:
        syslog(LOG_INFO, "stop work");
        break;
    default:
        break;
    }
}

void Client::openConn() {
    syslog(LOG_INFO, "try connect");
    connection.openConn(false);
    host_semaphore = sem_open(ConnHelper::SEM_HOST_NAME.c_str(), O_CREAT);
    client_semaphore = sem_open(ConnHelper::SEM_CLIENT_NAME.c_str(), O_CREAT);
    if (host_semaphore == SEM_FAILED){
        connection.closeConn();
        throw std::runtime_error("host semaphore open failed with error " + std::string(strerror(errno)));
    }
    if (client_semaphore == SEM_FAILED){
        sem_close(host_semaphore);
        connection.closeConn();
        throw std::runtime_error("client semaphore open failed with error " + std::string(strerror(errno)));
    }
    kill(host_pid, SIGUSR1);
}

void Client::termConn() {
    kill(host_pid, SIGUSR2);
    syslog(LOG_NOTICE, "terminate client");
    if (sem_close(client_semaphore) == -1 || sem_close(host_semaphore) == -1) {
        syslog(LOG_ERR, "close semaphore error : %s", strerror(errno));
    }
    connection.closeConn();
}

void Client::startClient(){
    char buf[ConnHelper::BUF_SIZE] = "dd.mm.yyyy";
    while (isWork) {
        ClientWait();
        connection.readConn(buf, ConnHelper::BUF_SIZE)
        std::cout << "Client received: " << buf << "\n";
        if (buf[0] == 'q') {
            isWork = false;
            break;
        }
        std::string dateString(buf);
        DTStor* DTSInst = DTStor::getDTStor(dateString);
        int weather = DTSInst->getTemp();
        std::sprintf(buf, "%i", weather);
        connection.writeConn(buf, ConnHelper::BUF_SIZE);
        std::cout << "Client wrote: " << buf << "\n";
        sem_post(host_semaphore);
    }
    syslog(LOG_INFO, "Client: exit message received");
}

void Client::ClientWait(){
    struct timespec ts; // for timeouts
    int rc = clock_gettime(CLOCK_REALTIME, &ts);
    if (rc == -1)
        throw std::runtime_error("Can't get current time for sem_timedwait");
    ts.tv_sec += ConnHelper::TIMEOUT;
    while ((rc = sem_timedwait(client_semaphore, &ts)) == EINTR);
    if (rc != -1)
        return;
    if (errno == ETIMEDOUT)
	    throw std::runtime_error("expired timeout");
}
