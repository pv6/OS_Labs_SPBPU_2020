#include "host_cls.h"

Host::Host() : hci(0) {
    struct sigaction structSigaction;
    memset(&structSigaction, 0, sizeof(structSigaction));
    structSigaction.sa_sigaction = signalHandler;
    structSigaction.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &structSigaction, nullptr);
    sigaction(SIGINT, &structSigaction, nullptr);
    sigaction(SIGUSR1, &structSigaction, nullptr);
    sigaction(SIGUSR2, &structSigaction, nullptr);
}

Host &Host::getInstance() {
    static Host instance();
    return instance;
}

void Host::openConn() {
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
}

void Host::signalHandler(int sig, siginfo_t *info, void *context)
{
    Host &instance = getInstance();
    switch (sig)
    {
        case SIGUSR1:
            syslog(LOG_NOTICE, "host: SIGUSR1 signal catched.");
            if (instance.connectionInfo.isAttached())
                syslog(LOG_INFO, "host can process one client");
            else{
                syslog(LOG_INFO, "Attaching client with pid = %i" ,info->si_pid);
                instance.hci = ConnectionInfo(info->si_pid);
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
            instance.terminate();
            break;
        case SIGINT:
            syslog(LOG_NOTICE, "host: interrupt signal catched.");
            instance.terminate();
            break;
    }
}

void Host::termConn() {
    syslog(LOG_NOTICE, "terminate client");
    if (sem_unlink(ConnectionConst::SEM_HOST_NAME.c_str()) == -1 ||
        sem_unlink(ConnectionConst::SEM_CLIENT_NAME.c_str()) == -1) {
        syslog(LOG_ERR, "sem_unlink error : %s", strerror(errno));
    }
    connection.closeConn();
}

void Host::startHost(const std::string& inputFile, const std::string& outputFile){
    std::ifstream infile(inputFile, std::ios::in);
    std::ofstream outfile(outputFile, std::ios::out);
    char buf[ConnHelper::BUF_SIZE] = "dd.mm.yyyy";
    try {  // have to close files before passing exception to main try-catch
    if (!infile.good())
        throw std::runtime_error("Host: can't open input file");
    if (!outfile.good())
        throw std::runtime_error("Host: can't open output file");
    std::string curLine;
    while (isWork && !infile.eof()) {
        if (!hci.isAttached()) {
            syslog(LOG_INFO, "wait client connect");
            sem_wait(client_semaphore);
            while (!hci.isAttached()) {
                pause();
            }
            if (!isWork) {
                return;
            }
            syslog(LOG_INFO, "client attached");
            sem_post(client_semaphore);
        } else{
            std::getline(infile, curLine);
            if (curLine.length() != 10) {
                isWork = false;
                break;
            }
            HostWait();
            std::cout << "Host writing: " << curLine << "\n";
            if (hci.isAttached())
                connection.writeConn(curLine.c_str(), curLine.length() + 1);
            sem_post(client_semaphore);
            HostWait();
            connection.readConn(buf, ConnHelper::BUF_SIZE);
            std::cout << "Host received: " << buf << "\n";
            outfile.write(buf, std::strlen(buf));
            outfile.write("\n", 1);
            sem_post(client_semaphore);
        }
    }
    syslog(LOG_INFO, "the last line processed in host");
    HostWait();  // wait until host can write
    connection.writeConn("qt_clt_msg", ConnHelper::BUF_SIZE);
	std::cout << "Host writing exit message\n";
    sem_post(client_semaphore);
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
