#ifndef DISKMONITOR_H
#define DISKMONITOR_H

#include <string>
#include <memory>
#include <unordered_map>
#include "config.h"

class DiskMonitor
{
public:
    void init( std::string const& configName );
    bool start();
    static DiskMonitor & get();
private:
    void runAll();

    DiskMonitor(){}
    DiskMonitor(DiskMonitor const&) = delete;
    DiskMonitor & operator=(DiskMonitor const&) = delete;
    void workLoop();
    void work();

    void handlePidFile();

    void fail( std::exception &&exc );

    void buildWatchDescrToPath();
    void removeWatches();

    const std::string tag = "DM_KD";
    const std::string pidFile = "/var/run/dm_kd.pid";

    static void signalHandle( int sigType );

    Config config;
    std::unordered_map<int, std::string> watchDescrToPath;
    std::vector<std::string> watchDirectories;
    size_t eventsCount;
    bool run; // true if work loop is available
    int inotifyDescr = -1;

    char *events = nullptr;
    size_t bufferSize;

    static DiskMonitor instance;
};

#endif // DISKMONITOR_H
