#include "my_handler.h"

void Daemon::SetSignalHandler(void (*_pSignalHandler)(int)) {
    DaemonBase::SetSignalHandler(SIG_DFL);
    signal(SIGHUP, _pSignalHandler);
    signal(SIGTERM, _pSignalHandler);

}

void Daemon::process()
{
    UpdateData();
    syslog(LOG_INFO, "Daemon's process is starting...");
    while(!_isFinished)
    {
        MoveFiles(MODE::TO_OLD);
        MoveFiles(MODE::TO_NEW);
        sleep(_timeSlice);
    }
    syslog(LOG_INFO, "Daemon's process was finished");

}

void Daemon::UpdateData() noexcept(false)
{
    _timeSlice = _pConfig->GetParam<long>("TIME_SLICE");
    _pathNew = _pConfig->GetParam<string>("PATH_1");
    _pathOld = _pConfig->GetParam<string>("PATH_2");
}

void Daemon::MoveFiles(MODE mode)
{
    string pathSrc, pathDst;
    if (mode == MODE::TO_OLD){
        pathSrc = _pathNew;
        pathDst = _pathOld;
    }
    else {
        pathSrc = _pathOld;
        pathDst = _pathNew;
    }

    DIR * dir = opendir(pathSrc.c_str());
    if (!dir) {
        throw std::runtime_error("Can't open directory");
    }

    struct stat fileStat;
    struct dirent * ent;

    time_t createTime, curTime;
    time(&curTime);

    while ((ent = readdir(dir)) != nullptr) {
        if (ent->d_type == DT_DIR) {
            continue;
        }
        std::string filePathSrc, filePathDst;
        std::stringstream ss;
        ss << pathSrc << "/" << ent->d_name;
        filePathSrc = ss.str();
        ss.str("");
        ss << pathDst << "/" << ent->d_name;
        filePathDst = ss.str();
        stat(filePathSrc.c_str(), &fileStat);
        createTime = fileStat.st_atime;
        if ( (mode == MODE::TO_OLD && (curTime - createTime) > CONSTANTS::MOVE_TIME_BOUND)
             || (mode == MODE::TO_NEW && (curTime - createTime) < CONSTANTS::MOVE_TIME_BOUND)){
            MoveFile(filePathSrc.c_str(), filePathDst.c_str());
        }
    }
    closedir(dir);
}

void Daemon::MoveFile(const char* pathSrc, const char* pathDst)
{
    int src = open(pathSrc, O_RDONLY, 0);
    int dst = open(pathDst, O_WRONLY | O_CREAT , 0644);

    struct stat statSrc;
    fstat(src, &statSrc);

    sendfile(dst, src, nullptr, statSrc.st_size);

    close(src);
    close(dst);

    if (std::remove(pathSrc) != 0) {
        throw std::runtime_error("Can't move the file");
    }
}
