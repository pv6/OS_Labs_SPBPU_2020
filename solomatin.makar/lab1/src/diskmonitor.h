#ifndef DISKMONITOR_H
#define DISKMONITOR_H

#include <map>
#include <syslog.h>
#include <vector>

using namespace std;

// disk monitoring logic encapsulated here, SINGLETON
class DiskMonitor {
    // main while loop stopper
    static bool runnable;

    struct Configuration {
        vector<string> watchPaths; // (watch descriptors + paths to directries)
        unsigned int maxEvents; // maximum number of buffered watches

        string toString();

        static Configuration * defaultConfig();
    } *config;
    map<int, string> pathMap;

    // inotify instance descriptor
    int inotifyFd = -1;

    // recursively add watches starting with directory
    void addWatches();

    // remove all active watches
    void removeWatches();

    // retrieve directories and max watches from file, PURE, MEMALLOC
    Configuration * createConfig(const string &configFile);

    DiskMonitor();
public:
    void applyConfig(const string &configFile);
    void run();
    void finish();

    static DiskMonitor & instance() {
        static DiskMonitor instance;
        return instance;
    }
    ~DiskMonitor();
};

#endif /*DISKMONITOR_H*/
