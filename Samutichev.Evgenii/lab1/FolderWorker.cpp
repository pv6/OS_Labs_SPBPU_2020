#include "FolderWorker.h"
#include "Error.h"
#include <dirent.h>
#include <errno.h>
#include <syslog.h>

FolderWorker::FolderWorker(const std::string& folder1Path, const std::string& folder2Path, size_t oldDefTime) {
    _folder1Path = folder1Path;
    _folder2Path = folder2Path;
    _oldDefTime = oldDefTime;
}

void FolderWorker::work() {
    DIR* dir1 = opendir(_folder1Path.c_str());
    if (!dir1) {
        /*struct dirent *ent;
            while ((ent = readdir (dir)) != NULL)
                std::cout << ent->d_name << "\n";
            closedir(dir);
            std::cout << "OK";*/
        if (ENOENT == errno)
            throw Error::NO_SUCH_FOLDER;
        else
            throw Error::UNKNOWN;
    }

    syslog(LOG_NOTICE, "Folder 1 succesfuly opened");

    DIR* dir2 = opendir(_folder2Path.c_str());
    if (!dir2) {
        if (ENOENT == errno) {
            throw Error::NO_SUCH_FOLDER;
        }
        else
            throw Error::UNKNOWN;
    }

    syslog(LOG_NOTICE, "Folder 2 succesfuly opened");
}
