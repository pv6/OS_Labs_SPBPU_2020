//
// Created by yudzhinnsk on 30.10.2020.
//
#ifndef UNTITLED1_DEMON_H
#define UNTITLED1_DEMON_H
#include <stdio.h>
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <pwd.h>
#include <string.h>
#include <dirent.h>
#include <iostream>
#include "NeededInfo.h"
#include "SubMethods.h"
class Demon{
public:
    Demon(NeededInfo &nInf);
    void createDemon();
    static void kill_last_daemon();
    static void signal_handler(int signum);
    int deleteSubFolders(std::string &path, int deepLevel);
    void runDemon();
private:
    static NeededInfo *nf;
};
#endif //UNTITLED1_DEMON_H
