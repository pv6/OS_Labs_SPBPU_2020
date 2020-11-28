#include "client_cls.h"

Client &Client::getInstance(int host_pid) {
    static Client instance(host_pid);
    return instance;
}

Client::Client(int host_pid){
    this->pid = host_pid;
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
        client.isWork = false;
        break;
    default:
        break;
    }
}

void Client::openConn() {
    syslog(LOG_INFO, "try connect");
    std::cout << "try connect" << std::endl;
    connection.openConn(false);
    host_semaphore = sem_open(ConnHelper::SEM_HOST_NAME.c_str(), O_CREAT);
    client_semaphore = sem_open(ConnHelper::SEM_CLIENT_NAME.c_str(), O_CREAT);
    if (host_semaphore == SEM_FAILED){
        std::cout << "try close host sem" << std::endl;
        connection.closeConn();
        throw std::runtime_error("host semaphore open failed with error " + std::string(strerror(errno)));
    }
    if (client_semaphore == SEM_FAILED){
        std::cout << "try close client sem" << std::endl;
        sem_close(host_semaphore);
        connection.closeConn();
        throw std::runtime_error("client semaphore open failed with error " + std::string(strerror(errno)));
    }
    kill(pid, SIGUSR1);
}

void Client::termClient() {
    kill(pid, SIGUSR2);
    syslog(LOG_NOTICE, "terminate client");
    if (sem_close(client_semaphore) == -1 || sem_close(host_semaphore) == -1) {
        syslog(LOG_ERR, "close semaphore error : %s", strerror(errno));
    }
    connection.closeConn();
}

void Client::startClient(){
    char buf[ConnHelper::BUF_SIZE] = "dd.mm.yyyy";
    syslog(LOG_NOTICE, "start client");
    //ClientWait();
    syslog(LOG_NOTICE, "start client cycle");
    isWork = true;
    while (isWork) {
        syslog(LOG_NOTICE, "client wait");
        sem_wait(client_semaphore);
        syslog(LOG_NOTICE, "client read");
        connection.readConn(buf, ConnHelper::BUF_SIZE);
        std::cout << "Client received: " << buf << "\n";
        syslog(LOG_NOTICE, "is quit");
        if (buf[0] == 'q') {
            isWork = false;
            break;
        }
        syslog(LOG_NOTICE, "buf to string");
        std::string dateString(buf);
        syslog(LOG_NOTICE, "get dtstor");
        DTStor* DTSInst = DTStor::getDTStor(dateString);
        syslog(LOG_NOTICE, "get temp");
        int weather = DTSInst->getTemp();
        std::cout << "Generated weather: " << weather << std::endl;
        std::sprintf(buf, "%i", weather);
        syslog(LOG_NOTICE, "client write");
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
