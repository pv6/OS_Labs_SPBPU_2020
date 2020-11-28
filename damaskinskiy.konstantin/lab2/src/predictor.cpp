#include <cstring>
#include <random>
#include <syslog.h>
#include <signal.h>
#include "predictor.h"
#include "conn.h"

Predictor Predictor::instance;

Predictor::Predictor()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = sigHandler;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGUSR1, &act, nullptr);
}

void Predictor::sigHandler( int signum, siginfo_t *si, void *ucontext )
{
    (void)si;
    (void)ucontext;

    switch (signum)
    {
    case SIGUSR1:
        instance.run = true;
        break;
    case SIGUSR2:
        instance.terminate();
        break;
    default:
        syslog(LOG_ERR, "Unhandled signal: %s", strsignal(signum));
    }
}

void Predictor::setHostPid( __pid_t pid )
{
    hostPid = pid;
}

bool Predictor::connectToHost()
{
    if (hostPid == std::numeric_limits<__pid_t>::max())
    {
        syslog(LOG_ERR, "Host pid is not mentioned");
        return false;
    }

    if (conn.open(getpid(), false)) {
        std::string semNameClient = "predictor" + std::to_string(getpid());
        std::string semNameHost = "host" + std::to_string(getpid());

        try
        {
            semPred.create(semNameClient.c_str());
            semHost.open(semNameHost.c_str());
        }
        catch (std::exception &)
        {
            syslog(LOG_ERR, "%s", strerror(errno));
            return false;
        }
        syslog(LOG_NOTICE, "Connection %i is set", getpid());
        return true;
    }
    return false;
}

void Predictor::predict()
{
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_int_distribution<int> distribution(-30, 40);
    int number = distribution(gen);
    conn.write(&number, sizeof(int));

    while (run)
    {
        semPred.decrement();
        conn.write(&number, sizeof(int));
        syslog(LOG_INFO, "Predictor %i predicts temperature %i", getpid(), number);

        semHost.increment();
    }
}

void Predictor::terminate()
{
    kill(hostPid, SIGUSR2);
    try
    {
        semPred.close();
        semHost.close();
        semHost.unlink();
    }
    catch (...) {
        syslog(LOG_ERR, "%s", strerror(errno));
        exit(errno);
    }
    if (!conn.close())
    {
        syslog(LOG_ERR, "%s", strerror(errno));
        exit(errno);
    }
    syslog(LOG_INFO, "Predictor %i is terminated", getpid());
    run = false;
}

Predictor & Predictor::get()
{
    return instance;
}
