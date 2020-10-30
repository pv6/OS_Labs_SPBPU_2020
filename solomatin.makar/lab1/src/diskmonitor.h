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
        unsigned int maxEvents; // maximum number of buffered events
        Configuration() : maxEvents(128) {}

        string toString();
        static Configuration * defaultConfig();
    } *config;

    // mapping from FD to file path; therefore paths duplicated in this and conig->watchPaths.
    map<int, string> pathMap;

    // inotify instance descriptor
    int inotifyFd = -1;

    // create watches by lookup config's "watchPaths" field, fill "pathMap" accordingly
    void addWatches();

    // remove all active watches, clean "pathMap" and config->watchPaths.
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
