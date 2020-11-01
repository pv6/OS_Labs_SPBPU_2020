#ifndef DAEMONMANAGER_H_INCLUDED__
#define DAEMONMANAGER_H_INCLUDED__

#include <string>

#include "../include/ConfigParser.h"
#include "../include/FileTransfer.h"


class DaemonManager final {
    public:
        static bool create_DaemonManager(std::string pid_path, std::string config_path);
        static void run();

    private:
        DaemonManager() = delete;

        static bool _update_config();

        static void _signal_handler(int signal);

        //static std::string _pid_path;
        //static std::string _config_path;
        static int _wait_time;

        static ConfigParser _cp;
        static FileTransfer _ft;

        static bool _status;
};

#endif