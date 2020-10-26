#ifndef DAEMON_H_INCLUDED
#define DAEMON_H_INCLUDED

#include "FolderWorker.h"
#include "Error.h"

class Daemon {
public:
    static Daemon* get();
    ~Daemon();

    void run(const char* configFilePath);

    static void handleSIGTERM(int signum);
    static void handleSIGHUP(int signum);

private:
    static Daemon* _instance;
    Daemon();
    Daemon(const Daemon& other) = delete;
    Daemon& operator=(const Daemon& other) = delete;

    void pidFileCheck();
    void initPidFile();
    void clearPidFile();

    FolderWorker* _folderWorker = nullptr;
    bool _active = true;
    std::string _configFilePath;
    size_t _updateTime;
    Error _lastError = Error::OK;
};

#endif // DAEMON_H_INCLUDED
