#include "FolderWorker.h"
#include "Error.h"
#include <dirent.h>
#include <errno.h>
#include <syslog.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

FolderWorker::FolderWorker(const std::string& folder1Path, const std::string& folder2Path, size_t oldDefTime) {
    _folder1Path = folder1Path;
    _folder2Path = folder2Path;
    _oldDefTime = oldDefTime;
}

void FolderWorker::work() {
    DIR* dir1 = opendir(_folder1Path.c_str());
    if (!dir1) {
        if (ENOENT == errno) {
            std::string msg = "Folder " + _folder1Path + " doesn't exists";
            syslog(LOG_ERR, msg.c_str());
            throw Error::NO_SUCH_FOLDER;
        }
        else
            throw Error::UNKNOWN;
    }

    if (!dirExists(_folder2Path.c_str())) {
        closedir(dir1);
        std::string msg = "Folder " + _folder2Path + " doesn't exists";
        syslog(LOG_ERR, msg.c_str());
        throw Error::NO_SUCH_FOLDER;
    }

    std::string folderNEWPath = _folder2Path + "/NEW";
    std::string folderOLDPath = _folder2Path + "/OLD";

    if (!dirExists(folderNEWPath)) {
        syslog(LOG_NOTICE, "NEW folder was created");
        system(("mkdir " + folderNEWPath).c_str());
    }

    if (!dirExists(folderOLDPath)) {
        syslog(LOG_NOTICE, "OLD folder was created");
        system(("mkdir " + folderOLDPath).c_str());
    }

    struct dirent* ent;
    struct stat buff;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    while ((ent = readdir(dir1)) != NULL) {
        std::string file = _folder1Path + "/" + ent->d_name;
        stat(file.c_str(), &buff);
        long long fileAge = tv.tv_sec - (long long)buff.st_mtim.tv_sec;
        syslog(LOG_NOTICE, "%s file %lld", file.c_str(), fileAge);
        if (fileAge > (long long)_oldDefTime)
            system(("cp " + file + " " + folderOLDPath).c_str());
        else
            system(("cp " + file + " " + folderNEWPath).c_str());
    }

    closedir(dir1);
}

bool FolderWorker::dirExists(const std::string& folderPath) const {
    DIR* dir = opendir(folderPath.c_str());
    if (!dir) {
        if (ENOENT == errno)
            return false;
        else
            throw Error::UNKNOWN;
    }

    closedir(dir);

    return true;
}
