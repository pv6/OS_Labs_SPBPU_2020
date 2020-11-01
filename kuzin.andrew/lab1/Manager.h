#ifndef MANAGER_H
#define MANAGER_H

#include <iostream>
#include <list>
#include <sstream>
#include <fstream>

class Manager {
public:
    static Manager &getInstance();

    void run();

private:
    Manager();

    Manager(Manager const &);

    Manager &operator=(Manager const &);

    std::list<std::string> commandList_;
    std::list<std::string> testEventList_;

    enum {
        COMMAND_ADD_EVENT,
        COMMAND_PRINT_CONFIG_AND_CHECK,
        COMMAND_RUN_DAEMON,
        COMMAND_EXIT
    } COMMAND;

    static void
    writeToConfigFile(const std::string &eventDate, const std::string &eventTime, const std::string &eventFlag,
                      const std::string &eventText);

    void addEvent();

    bool checkEventDateAndTime(const std::string &eventDate, const std::string &eventTime, bool needWriteToConsole);

    void printCommandList();

    void printConfigFile();

    static void runDaemon();

};


#endif //MANAGER_H
