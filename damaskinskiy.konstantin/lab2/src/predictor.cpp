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
    {
        syslog(LOG_INFO, "Host accepted connection!");
        break;
    }
    case SIGUSR2:
        instance.softTerminate();
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

    std::string semNameClient = "predictor" + std::to_string(getpid());
    try
    {
        semPred.create(semNameClient.c_str());
        kill(hostPid, SIGUSR1);
    }
    catch (std::exception &)
    {
        syslog(LOG_ERR, "Predictor semaphores error: %s", strerror(errno));
        return false;
    }
    syslog(LOG_INFO, "Connection %i is set", getpid());

    return conn.open(getpid(), true);
}

void Predictor::predict()
{
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_int_distribution<int> distribution(-30, 40);
    // TODO dependency on date
    int number = distribution(gen);

    try
    {
        // semaphore is not created =>
        // not all clients are connected to host => idle
        bool isSemReady = false;
        while (!isSemReady)
        {
            try
            {
                std::string semNameHost = "DK_forecast_host" + std::to_string(getpid());
                semHost.open(semNameHost.c_str());
                isSemReady = true;
            } catch (...) {}
        }

        while (run)
        {
            syslog(LOG_INFO, "Locked predictor pid %i", getpid());
            semPred.decrement();
            syslog(LOG_INFO, "Unlocked predictor pid %i", getpid());
            char date[11] = {0};
            conn.read(date, 10);
            syslog(LOG_INFO, "pid %i: read date %s", getpid(), date);
            memset(date, 0, 11);
            syslog(LOG_INFO, "Predictor %i predicts temperature %i for date %s",
                   getpid(), number, date);

            sprintf(date, "%i", number);
            syslog(LOG_INFO, "Sending %s...", date);
            conn.write(date, 3);
            syslog(LOG_INFO, "Predictor %i successfully sent prediction", getpid());

            semHost.increment();
        }
    } catch (std::exception &e)
    {
        syslog(LOG_ERR, "Error in predict method: %s", e.what());
        hardTerminate();
    }
}

void Predictor::softTerminate()
{
    try
    {
        semPred.close();
        semHost.close();
    }
    catch (...) {
        syslog(LOG_ERR, "Errors close semaphore: %s", strerror(errno));
        exit(errno);
    }
    if (!conn.close())
    {
        syslog(LOG_ERR, "Errors close connection: %s", strerror(errno));
        exit(errno);
    }
    syslog(LOG_INFO, "Predictor %i is terminated", getpid());
    run = false;
}

Predictor & Predictor::get()
{
    return instance;
}

void Predictor::hardTerminate()
{
    kill(hostPid, SIGUSR2);
    softTerminate();
}
