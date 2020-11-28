#include "my_client.h"
#include <semaphore.h>
#include <signal.h>

my_client::my_client(int host_pid_)
{
    host_pid = host_pid_;
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = signal_handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGINT, &act, nullptr);
}

my_client & my_client::get_instance(int host_pid) {
    static my_client client(host_pid);
    return client;
}

bool my_client::open_connection() {
    host_sem = sem_open(SEM_HOST_NAME, O_CREAT);
    if (host_sem == SEM_FAILED)
    {
        syslog(LOG_ERR, "client: couldn't open host semaphore.");
        return false;
    }
    client_sem = sem_open(SEM_CLIENT_NAME, O_CREAT);
    if (client_sem == SEM_FAILED)
    {
        syslog(LOG_ERR, "client: couldn't open client semaphore.");
        sem_close(host_sem);
        return false;
    }
    if (!conn.open_connect(host_pid, false))
    {
        syslog(LOG_ERR, "client: couldn't open conn.");
        sem_close(host_sem);
        sem_close(client_sem);
        return false;
    }
    kill(host_pid, SIGUSR1);
    return true;
}


void my_client::run() {
    message msg;
    while (true)
    {
        sem_wait(client_sem);
        if (conn.recv_connect(&msg, MESSAGE_SIZE))
        {
            srand(msg.day + msg.month + msg.year);
            int temp = rand() % 10 + 20;
            msg.temp = temp;
            std:: cout << "temperature for date " << msg.day << "." << msg.month << "." << msg.year << " is " << msg.temp << std::endl;
            syslog(LOG_NOTICE, "client: temperature for date %u.%u.%u is %d.", msg.day, msg.month, msg.year, msg.temp);
            conn.send_connect(&msg, MESSAGE_SIZE);
        }
        sem_post(host_sem);
    }
}

void my_client::signal_handler(int sig_pid, siginfo_t *info, void *context) {
    my_client &client = get_instance(0);
    client.terminate(sig_pid);
}

void my_client::terminate(int sig_pid) {
    kill(host_pid, SIGUSR1);
    syslog(LOG_NOTICE, "client: terminate");
    if (host_sem != SEM_FAILED)
    {
        if (sem_close(host_sem) == -1)
        {
            syslog(LOG_ERR, "client: fail to close host semaphore");
            exit(errno);
        }
    }
    if (client_sem != SEM_FAILED)
    {
        if (sem_close(client_sem) == -1)
        {
            syslog(LOG_ERR, "client: fail to close client semaphore");
            exit(errno);
        }
    }

    if (!conn.close_connect())
    {
        exit(errno);

    }

    syslog(LOG_ERR, "client: err while terminating");
    exit(EXIT_FAILURE);
}