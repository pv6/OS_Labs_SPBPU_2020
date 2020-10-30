#include <syslog.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <map>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <sys/inotify.h>
#include "diskmonitor.h"

bool DiskMonitor::runnable = true;
DiskMonitor::DiskMonitor() : config(nullptr) {
    if ((inotifyFd = inotify_init()) < 0) {
        syslog(LOG_ERR, "Could not initialize inotify");
        throw errno;
    }
    syslog(LOG_INFO, "Created inotify file");
}

string DiskMonitor::Configuration::toString() {
    string watchStr = "[";
    int n = watchPaths.size();
    for (int i = 0; i < n - 1; i++) watchStr += watchPaths[i] + ", ";
    if (n > 0) watchStr += watchPaths[n - 1] + "]";

    return "{watchPaths=" + watchStr + ", maxEvents=" + std::to_string(maxEvents) + "}";
}

DiskMonitor::Configuration * DiskMonitor::Configuration::defaultConfig() {
    Configuration *config = new Configuration;
    config->maxEvents = 128;
    config->watchPaths.push_back(getenv("HOME"));

    return config;
}

DiskMonitor::Configuration * DiskMonitor::createConfig(const string &fileName) {
    ifstream fin(fileName);

    if (!fin.is_open()) {
        syslog(LOG_INFO, "Could not open config file %s", fileName.c_str());
        return nullptr;
    }

    Configuration *config = new Configuration;
    string line;

    auto terminate = [config, &fin, &fileName](int i) {
        syslog(LOG_INFO, "Error while parsing config file %s at %i line", fileName.c_str(), i);
        fin.close();
        delete config;
    };

    for (int i = 1; getline(fin, line); i++) {
        istringstream iss(line);

        vector<string> words;
        string word;
        while (iss >> word) words.push_back(word);

        if (words.size() == 0 || words.at(0).at(0) == '#') continue;
        if (words.size() != 2) {
            terminate(i);
            return nullptr;
        }

        if (words.at(0) == "directory") {
            config->watchPaths.push_back(words.at(1));
        } else if (words.at(0) == "max_events") {
            config->maxEvents = std::stoi(words.at(1));
        } else {
            terminate(i);
            return nullptr;
        }
    }

    return config;
}

void DiskMonitor::applyConfig(const string &configFile) {
    Configuration *newConfig = createConfig(configFile);

    if (newConfig != nullptr) {
        syslog(LOG_INFO, "%s config was loaded: %s", configFile.c_str(), newConfig->toString().c_str());
        if (config != nullptr) {
            removeWatches();
            delete config;
        }
        config = newConfig;
        addWatches();
    } else {
        if (config == nullptr) {
            config = Configuration::defaultConfig();
            syslog(LOG_INFO, "Using default config: %s", config->toString().c_str());

            addWatches();
        }
    }
}

void DiskMonitor::removeWatches() {
    for (auto const &pair : pathMap) inotify_rm_watch(inotifyFd, pair.first);
    config->watchPaths.clear();
    pathMap.clear();
}

void DiskMonitor::addWatches() {
    for (string &path : config->watchPaths) {
        int wd = inotify_add_watch(inotifyFd, path.c_str(), IN_CREATE | IN_MODIFY | IN_DELETE);

        if (wd == -1) syslog(LOG_ERR, "Could not create watch on %s", path.c_str());
        else pathMap[wd] = path;
    }
}

DiskMonitor::~DiskMonitor() {
    if (runnable) finish();
}

void DiskMonitor::run() {
    const unsigned int maxFilePathLength = 16;
    const unsigned int eventSize = sizeof(inotify_event);
    const unsigned int bufferSize = config->maxEvents * (eventSize + maxFilePathLength);
    char *eventBuf = new char[bufferSize];
    while (runnable) {
        int length = read(inotifyFd, eventBuf, bufferSize);
        if (length < 0) {
            syslog(LOG_ERR, "Read from inotify returned %i", length);
            break;
        }

        inotify_event *event = (inotify_event *)eventBuf;
        for (int i = 0; i < length; i += eventSize + event->len, event = (inotify_event *)(eventBuf + i)) {
            if (event->len) {
                if (event->mask & IN_CREATE) {
                    if (event->mask & IN_ISDIR)
                        syslog(LOG_ERR, "%s DIR::%s CREATED\n", pathMap[event->wd].c_str(),event->name);
                    else
                        syslog(LOG_ERR, "%s FILE::%s CREATED\n", pathMap[event->wd].c_str(), event->name);
                }
                if (event->mask & IN_MODIFY) {
                    if (event->mask & IN_ISDIR)
                        syslog(LOG_ERR, "%s DIR::%s MODIFIED\n", pathMap[event->wd].c_str(),event->name);
                    else
                        syslog(LOG_ERR, "%s FILE::%s MODIFIED\n", pathMap[event->wd].c_str(),event->name);
                }
                if (event->mask & IN_DELETE) {
                    if (event->mask & IN_ISDIR)
                        syslog(LOG_ERR,"%s DIR::%s DELETED\n", pathMap[event->wd].c_str(),event->name );
                    else
                        syslog(LOG_ERR,"%s FILE::%s DELETED\n", pathMap[event->wd].c_str(),event->name );
                }
            }
        }
    }

    syslog(LOG_INFO, "Exited from endless while loop");
    delete[] eventBuf;
}

void DiskMonitor::finish() {
    runnable = false;

    removeWatches();
    syslog(LOG_INFO, "Removed all watches");

    close(inotifyFd);
    syslog(LOG_INFO, "Inotify file closed");

    if (config != nullptr) delete config;
    syslog(LOG_INFO, "Configuration deleted");
}