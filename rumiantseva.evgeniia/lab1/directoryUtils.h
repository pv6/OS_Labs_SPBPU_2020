//
// Created by Evgenia on 14.10.2020.
//

#ifndef INC_1_DIRECTORYUTILS_H
#define INC_1_DIRECTORYUTILS_H

#include <fstream>
#include <string>
#include <signal.h>
#include <dirent.h>
#include <syslog.h>
#include <sys/stat.h>
#include <unistd.h>

void CopyFile(const std::string &from, const std::string &to);

void CreateDir(const std::string &path);

DIR *OpenFolder(const std::string &path);

bool FolderExists(const std::string &path);

#endif //INC_1_DIRECTORYUTILS_H
