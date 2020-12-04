#include <syslog.h>
#include <sys/mman.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include "Host.h"
#include "../client/client.h"

const size_t commonID = 1010;

sem_t* create_semaphore(const std::string& name) {
    int memDescr = shm_open(name.c_str(), O_RDWR | O_CREAT, S_IRWXU);
    if (memDescr == -1)
        throw std::runtime_error("Failed to create shared memory" + std::string(strerror(errno)));

    if (ftruncate(memDescr, sizeof(sem_t)) == -1)
        throw std::runtime_error("Failed to truncate" + std::string(strerror(errno)));

    auto* semaphore = (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, memDescr, 0);
    if (semaphore == MAP_FAILED)
        throw std::runtime_error("Failed to map" + std::string(strerror(errno)));

    if (sem_init(semaphore, 1, 0) == -1)
        throw std::runtime_error("Failed to initialize semaphore" + std::string(strerror(errno)));
    return semaphore;
}

int main() {
    openlog("host_lab2", LOG_PID, LOG_DAEMON);
    try {
        sem_t* sem_host = create_semaphore(sem_host_name);
        sem_t* sem_client = create_semaphore(sem_client_name);
        pid_t pid = fork();
        if (pid < 0)
            throw std::runtime_error("Failed to fork " + std::string(strerror(errno)));
        if (pid == 0) {
            sleep(1);
            Client& client = Client::getInstance(commonID, sem_host, sem_client);
            client.openConnection();
            client.run();
        } else {
            Host& host = Host::getInstance(commonID);
            host.openConnection(sem_host, sem_client);
            host.run();
            kill(pid, SIGTERM);
            syslog(LOG_NOTICE, "Client process terminated");
            host.terminate();
        }

    } catch (std::exception& ex) {
        syslog(LOG_ERR, "%s", ex.what());
    }
    if (sem_unlink(sem_host_name.c_str()) == -1 ||
        sem_unlink(sem_client_name.c_str()) == -1) {
        syslog(LOG_ERR, "sem_unlink error : %s", strerror(errno));
    }
    closelog();
    return EXIT_SUCCESS;
}