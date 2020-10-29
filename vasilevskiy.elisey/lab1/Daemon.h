#ifndef LAB1_DAEMON_H
#define LAB1_DAEMON_H

#include <string>

class Daemon {
public:
    static bool init(std::string const &configFileName);

    static void run();

private:
    static std::string pid;
    static std::string input;
    static std::string output;
    static std::string extension;
    static std::string config;
    static std::string homedir;
    static unsigned int intval;
    static bool exit;

    static bool initTread();

    static void initSignals();

    static void signalHandler(int signal);

    static bool initPid();

    static void CheckPid();

    static void SavePid();

    static void cleanDst();

    static void copySrcToDst();

    static int removeFile(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb);

    static int copyFile(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb);

    static void readConfig();

    static void stop();
};


#endif //LAB1_DAEMON_H
