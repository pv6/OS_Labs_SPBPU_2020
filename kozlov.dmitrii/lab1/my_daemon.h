#pragma once
#include <dirent.h>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include "daemon_base.h"

class Daemon : public DaemonBase {
public:
    void SetSignalHandler(void (*_pSignalHandler)(int)) override;
    void UpdateData() noexcept(false);
    void process() noexcept(false) override;
    friend class DaemonBase;
private:
    void MoveFiles(MODE) noexcept(false);
    void MoveFile(const char* pathSrc, const char* pathDst) noexcept(false);
    Daemon() = default;
private:
    long _timeSlice = 10;
    string _pathNew, _pathOld;
};
using DaemonPtr = std::shared_ptr<Daemon>;

