#include <iostream>
#include <fstream>
#include <cstring>
#include <syslog.h>
#include <unistd.h>

#include "host.h"

std::shared_ptr<Host> Host::instance = std::make_shared<Host>();

void Host::sigHandler( int signum, siginfo_t* info, void* ptr ) {
    auto host = Host::get();
    switch (signum)
    {
    case SIGUSR1:
        if (host->predictorPid.find(info->si_pid) != host->predictorPid.end())
        {
            syslog(LOG_INFO, "Predictor pid %i is already a registered predictor",
                   info->si_pid);
            break;
        }
        if (host->predictorPid.size() == host->predictorCount)
        {
            syslog(LOG_ERR,
                   "Predictor pid %i cannot be connected because "
                   "%lu clients have been required",
                   info->si_pid, host->predictorCount);
            break;
        }
        host->predictorPid.insert(info->si_pid);
        break;
    default:
        syslog(LOG_INFO, "Received signal: %s", strsignal(signum));
    }
}

Host::Host() : semHost("host_dm_forecast")
{
    struct sigaction act;
    act.sa_sigaction = sigHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, nullptr);
}

void Host::setupPredictorCount(size_t count)
{
    predictorCount = count;
    predictorThr.resize(count);
}

void * Host::requestForecast( void *date_void )
{
    ThreadData *datum = static_cast<ThreadData *>(date_void);

    Conn conn(datum->predictorPid);
    Host::get()->semHost.timedDecrement();  // wait until host can write
    conn->write(datum->date.c_str(), datum->date.length());

    // get client semaphore
    Semaphore semClient;
    semClient.open("predictor" + std::to_string(datum->predictorPid));
    semClient.increment();  // let client read & write
    Host::get()->semHost.timedDecrement(); // wait until host can read
    void *answer;
    // get sizeof(int)
    conn->read(answer, sizeof(int));
    Host::get()->semHost.increment();
    syslog(LOG_INFO, "Host: predictor %i processed", datum->predictorPid);
    Host::get()->semHost.timedDecrement();  // wait until host can write
    semClient.increment();  // let client receive exit message
    semClient.close();
}

std::shared_ptr<Host> Host::get()
{
    return instance;
}

void Host::run( std::string const& date )
{
    // require predictions
    size_t idx = 0;
    for (auto &p : predictorPid)
    {
        ThreadData *datum = new ThreadData;
        datum->date = date;
        datum->predictorPid = p;
        pthread_create(&predictorThr[idx++], nullptr, requestForecast,
                       static_cast<void *>(datum));
    }

    // collect predictions
    std::vector<char *> predictions(predictorCount);
    idx = 0;
    for (auto &p : predictorThr)
    {
        void *pred;
        pthread_join(p, &pred);
        predictions[idx++] = static_cast<char *>(pred);
    }

    // output predictions
    idx = 0;
    for (auto &pid : predictorPid)
        std::cout << "Predictor " << pid << ": " << predictions[idx ] << "\n";
}

int main( int argc, char *argv[] )
{
    openlog("DK_forecast", LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_USER);
    if (argc != 3 || (argc >= 2 && strcmp(argv[1], "--clients")))
    {
        syslog(LOG_INFO, "Usage: "
                         "--clients client count\n");
        return -1;
    }

    size_t clientCount = std::stoul(argv[2]);

    auto host = Host::get();
    host->setupPredictorCount(clientCount);

    bool runForecast = true;
    while (runForecast)
    {
        std::cout << "Input date dd.mm.yyyy or 0 to exit: ";
        std::string date;
        std::cin >> date;

        if (date == "0")
            runForecast = false;
        else
            host->run(date);
    }

    return 0;
}
