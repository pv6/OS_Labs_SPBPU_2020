//
// Created by Evgenia on 14.10.2020.
//

#include "directoryUtils.h"

void CopyFile(const std::string &from, const std::string &to)
{
    std::ifstream fromStream(from);
    if (!fromStream.is_open()) {
        syslog(LOG_ERR, "Failed to open file %s (read)", from.c_str());
        return;
    }
    std::ofstream toStream(to);
    if (!toStream.is_open()) {
        syslog(LOG_ERR, "Failed to open file %s (write)", to.c_str());
        fromStream.close();
        return;
    }
    toStream << fromStream.rdbuf();
    fromStream.close();
    toStream.close();
}

bool FolderExists(const std::string &path)
{
    struct stat st;
    return !stat(path.c_str(), &st) && S_ISDIR(st.st_mode);
}

void CreateDir(const std::string &path)
{
    if (!FolderExists(path)) {
        int rc;
        if ((rc = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))) {
            syslog(LOG_ERR, "Folder %s was not created: %i", path.c_str(), rc);
            std::string errDescription = "Folder creation error";
            throw CustomException(ERROR_EXIT, errDescription);
        }
    }
}

DIR* OpenFolder(const std::string &path)
{
    DIR *folder = opendir(path.c_str());
    if (folder == nullptr) {
        syslog(LOG_ERR, "Directory %s was not opened", path.c_str());
        std::string errDescription = "Directory open error";
        throw CustomException(ERROR_EXIT, errDescription);
    }
    return folder;
}