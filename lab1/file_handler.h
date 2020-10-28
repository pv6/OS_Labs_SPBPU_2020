#ifndef FILE_HANDLER_H_INCLUDED
#define FILE_HANDLER_H_INCLUDED

#include <stdio.h>
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <syslog.h>
#include <pwd.h>
#include <ftw.h>
#include <string.h>
#include <sstream>
#include <glob.h>
#include <dirent.h>
#include <fcntl.h>


class file_handler{
public:
    int parse_config();
    static file_handler* getInstance(std::string name_config_file){
       if (!fh_instance)
         fh_instance = new file_handler(name_config_file);
       return fh_instance;
    }
    std::string getConfigName(){return this->name_config_file;}
    std::string getFolder(){return this->folder;}
    unsigned int getInterval(){return this->interval;}
    std::string getPIDFilePath(){return this->pid_file_path;}
private:
    file_handler(std::string name_config_file){this->name_config_file = name_config_file;}
    file_handler(const file_handler&);
    file_handler& operator=(file_handler&);
    static file_handler* fh_instance;
    //function for checking of exist
    bool is_existing_dir(std::string& path);
    bool is_existing_file(std::string& path);
    //constants:
    std::string name_config_file;
    std::string folder;
    unsigned int interval;
    const std::string pid_file_path = "/var/lab/lab_daemon_pid.pid";
};

#endif // FILE_HANDLER_H_INCLUDED
