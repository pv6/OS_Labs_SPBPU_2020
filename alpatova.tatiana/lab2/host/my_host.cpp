#include "my_host.h"

my_host::my_host()
{
    client_pid = 0;
    is_client_attached = false;
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = signal_handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGINT, &act, nullptr);
    sigaction(SIGUSR1, &act, nullptr);
};

my_host & my_host::get_instance() {
    static my_host host;
    return host;
}


bool my_host::open_connection() {
    client_sem= sem_open(SEM_CLIENT_NAME, O_CREAT, 0666, 0);
    if (client_sem == SEM_FAILED)
    {
        syslog(LOG_ERR, "host:  can't open client semaphore");
        return false;
    }
    host_sem = sem_open(SEM_HOST_NAME, O_CREAT, 0666, 0);
    if (host_sem == SEM_FAILED)
    {
        syslog(LOG_ERR, "host:  can't open host semaphore");
        sem_unlink(SEM_CLIENT_NAME);
        return false;
    }

    if (!conn.open_connect(getpid(), true))
    {
        syslog(LOG_ERR, "host:  can't open conn");
        sem_unlink(SEM_CLIENT_NAME);
        sem_unlink(SEM_HOST_NAME);
        return false;
    }
    syslog(LOG_NOTICE, "host:  successfully created");
    return true;
}

bool my_host::get_msg(message &msg) {
    std::string date, cur_sym;

    std::cin.clear();
    std::cin >> date;

    if (date.length() == 0)
        return false;

    std::vector<int> date_vec;
    std::istringstream ss(date);
    while (std::getline(ss, cur_sym, '.')) {
        try {
            unsigned t = std::stoul(cur_sym);
            date_vec.push_back(t);
        }
        catch (const std::exception &e) {
            std::cout << "ERROR: incorrect date" << std::endl;
            return false;
        }
    }

    if (!is_client_attached)
        return false;

    if (date_vec.size() != 3) {
        std::cout << "ERROR: wrong date format" << std::endl;
        return false;
    }

    msg = {date_vec[0], date_vec[1], date_vec[2]};
    return true;
}

void my_host::run() {
    syslog(LOG_NOTICE, "host:  start running");

    message msg;
    int err_code;
    struct timespec ts;
    while (true)
    {
        if (!is_client_attached)
            sleep(1);
        else
        {
            if (is_client_attached && !get_msg(msg))
                continue;
            if (!is_client_attached)
                continue;
            #ifndef host_sock
            conn.send_connect(&msg, MESSAGE_SIZE);
            #else
            if (!conn.send(&msg, MESSAGE_SIZE) && errno == EPIPE)
            {
                if (!conn.open(getpid(), true)){
                    terminate();
                    }
                    exit(EXIT_FAILURE);
                }

                conn.send(&msg, MESSAGE_SIZE);
            }
            #endif
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += TIMEOUT;
            sem_post(client_sem);
            while ((err_code = sem_timedwait(host_sem, &ts)) == -1 && errno == EINTR)
                continue;
            if (err_code == -1)
            {
                if (is_client_attached){
                    kill(client_pid, SIGTERM);
                    syslog(LOG_NOTICE, "host:  kill client");
                }
                client_pid = 0;
                is_client_attached = false;
            }
            else if (conn.recv_connect(&msg, MESSAGE_SIZE))
            {
                std:: cout << "temperature is " << msg.temp << std::endl;
                syslog(LOG_NOTICE, "host: temperature is %d.", msg.temp);
            }
        }
    }
}

void my_host::terminate()
{

    syslog(LOG_NOTICE, "host: host have terminated");
    if (is_client_attached)
        kill(client_pid, SIGTERM);
    if (conn.close_connect() && sem_unlink(SEM_CLIENT_NAME ) == 0 && sem_unlink(SEM_HOST_NAME))
    {
        exit(EXIT_FAILURE);
    }
    closelog();
    exit(errno);
}

void my_host::signal_handler(int sig, siginfo_t *info, void *context)
{
    my_host &instance = get_instance();
    switch (sig)
    {
        case SIGUSR1:
            syslog(LOG_NOTICE, "host: SIGUSR1 signal catched.");
            if (instance.client_pid == info->si_pid) {
                instance.client_pid = 0;
                instance.is_client_attached = false;
            }
            else
            {
                syslog(LOG_NOTICE, "host: attaching client with pid %d.", info->si_pid);
                if (instance.is_client_attached)
                    kill(instance.client_pid, SIGTERM);
                instance.client_pid = info->si_pid;
                instance.is_client_attached = true;
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