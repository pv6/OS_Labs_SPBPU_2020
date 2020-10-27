
#ifndef LAB1_DAEMON_H
#define LAB1_DAEMON_H


#include <string>
using namespace std;

class daemon
{
public:
    static bool startDaemonization(const string& configFileParse);
private:
    static void clearDaemon();
    static void terminateDaemon();
    static string getFullPath(const string& path);
    static void loadConfig();
    static bool checkPidFile();
    static bool setPidFile();
    static void signalHandler(int signalNum);
    static void startWork();
    static void histLogWork();
    static void listDirFile(ofstream& histLog, const string& path, int inTab);

    static string configFile;
    static string pidFileName;
    static string dirHome;
    static string dir1;
    static string dir2;
    static unsigned int timeInter;
    static string histLogFile;
};


#endif //LAB1_DAEMON_H
