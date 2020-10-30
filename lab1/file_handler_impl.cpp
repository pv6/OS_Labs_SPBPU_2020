#include "file_handler.h"

int file_handler::parse_config(){
    if (!is_existing_file(name_config_file)) {
        syslog (LOG_WARNING, "WARNING: Config file with name %s in not exist.", name_config_file.c_str());
        return EXIT_FAILURE;
    }
    std::ifstream config_file(name_config_file.c_str());
    if (config_file.is_open() && !config_file.eof()) {
        interval = 0;
        config_file >> folder >> interval;
        config_file.close();
        name_config_file = realpath(name_config_file.c_str(), NULL);
    }
    if (folder.length() == 0 || interval == 0) {
        syslog (LOG_WARNING, "WARNING: Args is not read. Please, check config file: %s.", name_config_file.c_str());
        return EXIT_FAILURE;
    }

    if (!is_existing_dir(folder)) {
        syslog (LOG_WARNING, "WARNING: Directory with name %s in not exist.", name_config_file.c_str());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool file_handler::is_existing_dir(std::string& path){
    if (path[0] == '~') {
         passwd *pw;
         uid_t uid;

        uid = geteuid();
        pw = getpwuid(uid);
        if (pw)
            path.replace(0, 1, std::string("/home/") + pw->pw_name);
        else
            syslog (LOG_WARNING, "WARNING: Couldn't find username by UID %u. There is no guarantee to find folder which path contains `~`.", uid);

    }
    struct stat buffer;
    if (path.length() != 0 && stat(path.c_str(), &buffer) == 0 && (S_ISDIR(buffer.st_mode)))
        return true;
    return false;
}

bool file_handler::is_existing_file(std::string& path){
    struct stat buffer;
    if (path.length() != 0 && stat(path.c_str(), &buffer) == 0)
        return true;
    return false;
}

file_handler* file_handler::fh_instance = nullptr;
