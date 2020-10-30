#ifndef DISKMONITOR_H
#define DISKMONITOR_H

#include <string>
#include <memory>
#include "config.h"

class DiskMonitor
{
public:
    void init( std::string const& configName );
    bool start();
    static DiskMonitor & get();
private:
    DiskMonitor(){}
    void workLoop();
    void work();

    void handlePidFile();

    const std::string tag = "DM_KD";
    const std::string pidFile = "/var/run/dm_kd.pid";

    static void signalHandle( int sigType );

    Config config;
    std::vector<std::string> watchDirectories;
    bool run; // true if work loop is available
    static DiskMonitor instance;
};

#endif // DISKMONITOR_H
