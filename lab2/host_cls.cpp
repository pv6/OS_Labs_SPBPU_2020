#include "host_cls.h"

Host::Host() : hci(0) {
    struct sigaction structSigaction{};
    memset(&structSigaction, 0, sizeof(structSigaction));
    structSigaction.sa_sigaction = signalHandler;
    structSigaction.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &structSigaction, nullptr);
    sigaction(SIGINT, &structSigaction, nullptr);
    sigaction(SIGUSR1, &structSigaction, nullptr);
    sigaction(SIGUSR2, &structSigaction, nullptr);
}

Host &Host::getInstance() {
    static Host instance;
    return instance;
}

void Host::openConn() {
    syslog(LOG_INFO, "try connect");
    std::cout << "try connect" << std::endl;
    connection.openConn(true);
    std::cout << "after connection" << std::endl;
    sem_unlink(ConnHelper::SEM_HOST_NAME.c_str());
    sem_unlink(ConnHelper::SEM_CLIENT_NAME.c_str());
    host_semaphore = sem_open(ConnHelper::SEM_HOST_NAME.c_str(), O_CREAT, 0777, 1);
    client_semaphore = sem_open(ConnHelper::SEM_CLIENT_NAME.c_str(), O_CREAT, 0777, 1);
    if (host_semaphore == SEM_FAILED){
        std::cout << std::string(strerror(errno)) << std::endl;
        connection.closeConn();
        throw std::runtime_error("host semaphore open failed with error " + std::string(strerror(errno)));
    }
    if (client_semaphore == SEM_FAILED){
        std::cout << std::string(strerror(errno)) << std::endl;
        std::cout << 232 << std::endl;
        sem_close(host_semaphore);
        std::cout << 34 << std::endl;
        connection.closeConn();
        std::cout << 36 << std::endl;
        throw std::runtime_error("client semaphore open failed with error " + std::string(strerror(errno)));
    }
}

void Host::signalHandler(int sig, siginfo_t *info, void *context)
{
    Host &instance = getInstance();
    switch (sig)
    {
        case SIGUSR1:
            syslog(LOG_NOTICE, "host: SIGUSR1 signal catched.");
            if (instance.hci.isAttached())
                syslog(LOG_INFO, "host can process one client");
            else{
                syslog(LOG_INFO, "Attaching client with pid = %i" ,info->si_pid);
                instance.hci = HostConnInfo(info->si_pid);
            }
            break;
        case SIGUSR2:
            syslog(LOG_NOTICE, "host: SIGUSR1 signal catched.");
            if (instance.hci.getPid() == info->si_pid) {
                syslog(LOG_NOTICE, "host: disconnect client.");
                instance.hci = HostConnInfo(0);
            }
            break;
        case SIGTERM:
            syslog(LOG_NOTICE, "host: terminate signal catched.");
            instance.termHost();
            break;
        case SIGINT:
            syslog(LOG_NOTICE, "host: interrupt signal catched.");
            instance.termHost();
            break;
    }
}

void Host::termHost() {
    syslog(LOG_NOTICE, "terminate client");
    if (sem_unlink(ConnHelper::SEM_HOST_NAME.c_str()) == -1 ||
        sem_unlink(ConnHelper::SEM_CLIENT_NAME.c_str()) == -1) {
        syslog(LOG_ERR, "sem_unlink error : %s", strerror(errno));
    }
    connection.closeConn();
}

void Host::startHost(const char* inputFile, const char* outputFile){
    syslog(LOG_NOTICE, "host start");
    std::ifstream infile(inputFile, std::ios::in);
    std::ofstream outfile(outputFile, std::ios::out);
    char buf[ConnHelper::BUF_SIZE] = "dd.mm.yyyy";
    try {
    if (!infile.good())
        throw std::runtime_error("Host: can't open input file");
    if (!outfile.good())
        throw std::runtime_error("Host: can't open output file");
    std::string curLine;
    //syslog(LOG_NOTICE, "host go to pause");
    isWork = true;
   // syslog(LOG_NOTICE, "post to client sem");
    while (isWork && !infile.eof()) {
        if (!hci.isAttached()) {
            std::cout << "wait client connect" << std::endl;
            syslog(LOG_INFO, "wait client connect");
            while (!hci.isAttached()) {
                std::cout << "pause" << std::endl;
                pause();
            }
            if (!isWork) {
                std::cout << "is not work" << std::endl;
                kill(hci.getPid(), SIGTERM);
                hci = HostConnInfo(0);
                return;
            }
            std::cout << "client attached" << std::endl;
            syslog(LOG_INFO, "client attached");
        } else{
            syslog(LOG_NOTICE, "host read from file");
            std::getline(infile, curLine);
            if (curLine.length() != ConnHelper::BUF_SIZE - 1) {
                isWork = false;
                break;
            }
            if (hci.isAttached()){
                memcpy(buf, curLine.c_str(), ConnHelper::BUF_SIZE);
                HostWait();
                connection.writeConn(buf, ConnHelper::BUF_SIZE);
            }
            syslog(LOG_NOTICE, "post client sem");
            sem_post(client_semaphore);
            syslog(LOG_NOTICE, "host wait");
            HostWait();
            syslog(LOG_NOTICE, "host read");
            connection.readConn(buf, ConnHelper::BUF_SIZE);
            std::cout << "Host received: " << buf << "\n";
            outfile.write(buf, std::strlen(buf));
            outfile.write("\n", 1);
            sem_post(host_semaphore);
        }
    }
    memcpy(buf, "qt_clt_msg", ConnHelper::BUF_SIZE);
    //syslog(LOG_NOTICE, "host wait");
    HostWait();
    //syslog(LOG_NOTICE, "host write");
    connection.writeConn(buf, ConnHelper::BUF_SIZE);
    syslog(LOG_NOTICE, "post client sem");
    sem_post(client_semaphore);
    HostWait();
    syslog(LOG_INFO, "the last line processed in host");
    std::cout << "Host writing exit message\n";
    } catch(std::runtime_error& err) {
        infile.close();
        outfile.close();
        throw err;
    }
    infile.close();
    outfile.close();
}

void Host::HostWait(){
    struct timespec ts; // for timeouts
    int rc = clock_gettime(CLOCK_REALTIME, &ts);
    if (rc == -1)
        throw std::runtime_error("Can't get current time for sem_timedwait");
    ts.tv_sec += ConnHelper::TIMEOUT;
    while ((rc = sem_timedwait(host_semaphore, &ts)) == EINTR);
    if (rc != -1)
        return;
    if (errno == ETIMEDOUT){
        syslog(LOG_INFO, "try turn off client");
        kill(hci.getPid(), SIGTERM);
        hci = HostConnInfo(0);
	    throw std::runtime_error("expired timeout");
    }
}
