#ifndef DISKMONITOR_H
#define DISKMONITOR_H

#include <string>

class DiskMonitor
{
public:
    DiskMonitor( std::string const &configName );

    void start();
private:
    Config config;
};

#endif // DISKMONITOR_H
