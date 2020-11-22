#ifndef DirectoryCleaner_HPP
#define DirectoryCleaner_HPP

#include <string>

class DirectoryCleaner
{
public:
    static DirectoryCleaner& instance(const std::string& configPath);
    void run();
private:
    static DirectoryCleaner& instance();

    const unsigned int scndsBtwnExecutions = 10;
    const std::string  loggingTag{"DIR_CLEANER"};
    const std::string  pidFilePath{"/var/run/dir_cleaner.pid"};
    
    std::string dir;
    std::string configPath;
    bool        isRunning;
    
    static void reloadConfig(int signalId);
    static void terminateExecution(int signalId);
    void ensureUnqnessViaPid();
    void rmvTmpFilesInDir();

    explicit DirectoryCleaner();
    ~DirectoryCleaner();
    DirectoryCleaner(const DirectoryCleaner&)= delete;
    DirectoryCleaner& operator=(const DirectoryCleaner&)= delete;
};

#endif //DirectoryCleaner_HPP
