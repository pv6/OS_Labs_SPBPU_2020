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
    {
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
        syslog(LOG_INFO, "Received signal from predictor %i", info->si_pid);
        host.connPredCount++;
        host.predictorPid.insert(info->si_pid);

        try
        {
            Conn conn;
            conn.open(info->si_pid, false);
            syslog(LOG_INFO, "Successfully connected to %i", info->si_pid);
        }
        catch (std::exception &e)
        {
            syslog(LOG_ERR, "Host couldn't initialize connection channel: %s",
                   e.what());
        }

        break;
    }
    case SIGUSR2:
        host.hardTerminate();
        break;
    case SIGTERM:
        host.softTerminate();
        break;
    default:
        syslog(LOG_INFO, "Received signal: %s", strsignal(signum));
    }
}

void Host::hardTerminate()
{
    for (auto &p : predictorThr)
        pthread_cancel(p);
}

Host::Host()
{
    struct sigaction act;
    act.sa_sigaction = sigHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, nullptr);
}

Host::~Host()
{
}

void Host::softTerminate()
{
    hardTerminate();
    for (auto &p : predictorPid)
        kill(p, SIGUSR2);
}

void Host::setupPredictorCount(size_t count)
{
    estPredCount = count;
    predictorThr.resize(count);
}

void * Host::requestForecast( void *date_void )
{
    ThreadData *datum = static_cast<ThreadData *>(date_void);

    auto &host = Host::get();

    try
    {
        Conn conn;
        conn.open(datum->predictorPid, true);
        //Host::get().semHost.timedDecrement();  // wait until host can write
        conn.write(
                    const_cast<void *>(reinterpret_cast<const void*>(datum->date.c_str())),
                    datum->date.length());

        // get client semaphore
        Semaphore semClient;
        semClient.open("predictor" + std::to_string(datum->predictorPid));
        semClient.increment();  // let client read & write
        host.semHost.timedDecrement(); // wait until host can read

        char *answer = new char[11]{0};
        // get sizeof(int)
        conn.read(answer, 10);
        syslog(LOG_INFO, "Host: predictor %i predicted %s", datum->predictorPid, answer);

        return answer;
    }
    catch (std::exception &e)
    {
        syslog(LOG_ERR, "Request forecast: exception caught: %s",
               e.what());
        return nullptr;
    }
}

Host & Host::get()
{
    return instance;
}

void Host::run( std::string const& date )
{
    // waiting for predictors attach
    while (connPredCount != estPredCount)
        ;

    // require predictions
    std::vector<char *> predictions(estPredCount);
    size_t idx = 0;

    for (auto &p : predictorPid)
    {
        auto semHostName = "DK_forecast_host" + std::to_string(p);

        // open or create needed host semaphore
        // for first date request -- creation.
        // later -- just open.
        try
        {
            semHost.tryOpen(semHostName);
        }
        catch (std::exception &e)
        {
            syslog(LOG_ERR, "Host semaphore creation failed: %s", e.what());
            softTerminate();
            exit(EXIT_FAILURE);
        }

        syslog(LOG_INFO, "Started thread creation");
        ThreadData *datum = new ThreadData;
        datum->date = date;
        datum->predictorPid = p;
//        pthread_create(&predictorThr[idx++], nullptr, requestForecast,
//                       static_cast<void *>(datum));
         predictions[idx++] = static_cast<char*>(requestForecast(static_cast<void *>(datum)));
    }

//    // collect predictions
//    idx = 0;
//    for (auto &p : predictorThr)
//    {
//        void *pred;
//        pthread_join(p, &pred);
//        predictions[idx++] = static_cast<char *>(pred);
//    }

    syslog(LOG_ERR, "All threads finished");

    // output predictions
    idx = 0;
    for (auto &pid : predictorPid)
    {
        std::cout << "Predictor " << pid << ": " << predictions[idx] << "\n";
        delete []predictions[idx++];
    }
}

int main( int argc, char *argv[] )
{
    openlog("DK_forecast_host", LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_USER);
    if (argc != 5)
    {
        syslog(LOG_INFO, "Usage: "
                         "--predictors predictor_count --file file\n");
        return -1;
    }

    if (strcmp(argv[1], "--predictors") || strcmp(argv[3], "--file"))
    {
        syslog(LOG_INFO, "Usage: "
                         "--predictors predictor_count --file file\n");
        return -1;
    }

    /* Collect dates */
    std::ifstream ifs(argv[4]);
    if (!ifs)
    {
        syslog(LOG_ERR, "Couldn't find file %s", argv[4]);
        return -1;
    }

    std::vector<std::string> dates;
    while (ifs)
    {
        std::string d;
        ifs >> d;
        if (d.size() != 10)
            continue;
        dates.push_back(d);
    }

    /* Read predictors count */
    size_t clientCount = std::stoul(argv[2]);

    auto &host = Host::get();
    host.setupPredictorCount(clientCount);

    for (auto &date: dates)
    {
        /* Per date processing */
        syslog(LOG_INFO, "Predicting date: %s...", date.c_str());
        host.run(date);
    }

    host.softTerminate();

    return 0;
}
