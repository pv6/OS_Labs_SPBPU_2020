//
// Created by yudzhinnsk on 30.10.2020.
//
#include <iostream>
#include "NeededInfo.h"
#include "Demon.h"
#include "SubMethods.h"
bool SubMethods::is_dir_exist(std::string& path) {
    if (path[0] == '~') {
        passwd *pw;
        uid_t uid;

        uid = geteuid();
        pw = getpwuid(uid);
        if (pw)
            path.replace(0, 1, std::string("/home/") + pw->pw_name);
        else
            syslog(LOG_WARNING, "WARNING: Couldn't find username by UID %u. There is no guarantee to find folder which path contains `~`.", uid);

    }
    struct stat buffer;
    if (path.length() != 0 && stat(path.c_str(), &buffer) == 0 && (S_ISDIR(buffer.st_mode)))
        return true;
    return false;
}
bool SubMethods::is_file_exist(const std::string &name) {
    struct stat buffer;
    if (name.length() != 0 && stat(name.c_str(), &buffer) == 0)
        return true;
    return false;
}
int SubMethods::read_config_file(NeededInfo &nf) {
    std::string config_path = nf.retConfigPath();
    if (!is_file_exist(config_path.c_str())) {
        printf("Config file %s does not exist. \n", config_path.c_str());
        return EXIT_FAILURE;
    }
    std::string file = "";
    unsigned int val = 0;
    std::ifstream config_file(config_path.c_str());
    if (config_file.is_open() && !config_file.eof()) {
        config_file >> file >> val;
        config_file.close();
        config_path = realpath(config_path.c_str(), NULL);
    }
    if (file.length() == 0 || val == 0) {
        printf("Empty args, please check config file %s. \n", config_path.c_str());
        return EXIT_FAILURE;
    }

    nf.setFolderPath(file);
    nf.setIntervalVal(val);

    if (!is_dir_exist(file)) {
        printf("Directory %s does not exist. \n", nf.retFolderPath().c_str());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

