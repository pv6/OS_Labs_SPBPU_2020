//
// Created by aleksandr on 27.10.2020.
//

#ifndef LAB1_DAEMON_H
#define LAB1_DAEMON_H

#include <string>
#include <vector>

class Daemon
{
public:

    static bool init(const std::string &config_file);
    static void run();
    static void terminate();

private:
    static const std::string title_;
    static std::string pidFile_;
    static std::string dir1_;
    static std::string dir2_;
    static std::string homeDir_;
    static std::string configFile_;
    static std::string totalLog_;
    static unsigned int period_;
    static bool status_;

    static bool setConfig(const std::string &configFile);
    static bool readConfig();
    static bool handlePidFile();
    static bool setPidFile();
    static bool doWork();
    static void signalHandler(int sigNum);
    static std::string getAbsPath(const std::string &path);
    static std::vector<std::string> getAllLogs(const std::string &path);
    static void writeLog(const std::string &log, std::ofstream &totalLog );
};

#endif //LAB1_DAEMON_H
