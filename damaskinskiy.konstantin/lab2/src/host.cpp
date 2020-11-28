#include <iostream>
#include <fstream>
#include <cstring>
#include <syslog.h>
#include <sys/stat.h>
#include <unistd.h>

#include "host.h"

Host Host::instance;

void Host::sigHandler( int signum, siginfo_t* info, void* ptr ) {
    auto &host = Host::get();
    switch (signum)
    {
    case SIGUSR1:
        if (host.predictorPid.find(info->si_pid) != host.predictorPid.end())
        {
            syslog(LOG_INFO, "Predictor pid %i is already a registered predictor",
                   info->si_pid);
            break;
        }
        if (host.predictorPid.size() == host.estPredCount)
        {
            syslog(LOG_ERR,
                   "Predictor pid %i cannot be connected because "
                   "%lu clients have been required",
                   info->si_pid, host.estPredCount);
            break;
        }
        host.connPredCount++;
        host.predictorPid.insert(info->si_pid);
        break;
    default:
        syslog(LOG_INFO, "Received signal: %s", strsignal(signum));
    }
}

void Host::terminate()
{
    for (auto &p : predictorThr)
        pthread_cancel(p);
    for (auto &p : predictorPid)
        kill(p, SIGUSR2);
}

Host::Host()
{
    try
    {
        semHost.create("host_dm_forecast");
    }
    catch (std::exception &e)
    {
        syslog(LOG_ERR, "Host semaphore creation failed: %s", e.what());
        exit(EXIT_FAILURE);
    }
    struct sigaction act;
    act.sa_sigaction = sigHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, nullptr);
}

Host::~Host()
{
    semHost.unlink();
}

void Host::setupPredictorCount(size_t count)
{
    estPredCount = count;
    predictorThr.resize(count);
}

void * Host::requestForecast( void *date_void )
{
    ThreadData *datum = static_cast<ThreadData *>(date_void);

    Conn conn;
    conn.open(datum->predictorPid, true); // ???
    Host::get().semHost.timedDecrement();  // wait until host can write
    conn.write(
                const_cast<void *>(reinterpret_cast<const void*>(datum->date.c_str())),
                datum->date.length());

    // get client semaphore
    Semaphore semClient;
    semClient.open("predictor" + std::to_string(datum->predictorPid));
    semClient.increment();  // let client read & write
    Host::get().semHost.timedDecrement(); // wait until host can read

    char *answer = new char[10];
    // get sizeof(int)
    conn.read(answer, sizeof(int));
    Host::get().semHost.increment();
    syslog(LOG_INFO, "Host: predictor %i processed", datum->predictorPid);
    Host::get().semHost.timedDecrement();  // wait until host can write
    semClient.increment();  // let client receive exit message
    semClient.close();

    return answer;
}

Host & Host::get()
{
    return instance;
}

void Host::run( std::string const& date )
{
    if (connPredCount != estPredCount)
    {
        std::cout << "Estimated: " <<  estPredCount << " predictors\n";
        std::cout << "Connected: " << connPredCount << " predictors\n";
    }
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
    std::vector<char *> predictions(estPredCount);
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
    openlog("DK_forecast_host", LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_USER);
    if (argc != 3)
    {
        syslog(LOG_INFO, "Usage: "
                         "--predictors predictor count\n");
        return -1;
    }

    if (argc >= 2 && strcmp(argv[1], "--predictors"))
    {
        syslog(LOG_INFO, "Usage: "
                         "--predictors predictor count\n");
        return -1;
    }

    size_t clientCount = std::stoul(argv[2]);

    auto &host = Host::get();
    host.setupPredictorCount(clientCount);

    bool runForecast = true;
    while (runForecast)
    {
        std::cout << "Input date dd.mm.yyyy or 0 to exit: ";
        std::string date;
        std::cin >> date;

        if (date == "0")
            runForecast = false;
        else
            host.run(date);
    }

    return 0;
}
