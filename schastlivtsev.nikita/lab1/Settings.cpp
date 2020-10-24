#include "Settings.h"
#include "Helpers.h"
#include <iostream>
#include <fstream>
#include <exception>
#include <syslog.h>

Settings::Settings()
{
    //ctor
}

Settings::~Settings()
{
    //dtor
    if (pidPath) {
        delete [] pidPath;
        pidPath = nullptr;
    }
    if (folderDest) {
        delete [] folderDest;
    }
    if (folderSrc) {
        delete [] folderSrc;
    }
}

// private section

void Settings::parseConfig(char const * const filename) {
    std::ifstream config(filename, std::ios_base::in);

    std::string confField;
    std::string confValue;
    std::string strLine;
    char delimeter = ' ';  // the delimeter is a single space
    int delimeterPos = 0;
    while (std::getline(config, strLine)) {
        delimeterPos = strLine.find(delimeter);
        confField = std::string(strLine).erase(delimeterPos, std::string::npos);
        confValue = std::string(strLine).erase(0, delimeterPos + 1);

        // grammar
        static const std::string pidPathLex("PID_PATH");
        static const std::string folderSrcLex("FOLDER_SRC");
        static const std::string folderDestLex("FOLDER_DEST");
        static const std::string waitLex("WAIT_SEC");
        static const std::string totalLogLex("TOTAL_LOG_NAME");

        if (confField == pidPathLex) { // it's specific pid path
            Helpers::strToCStr(&pidPath, confValue);
        }
        else if (confField == folderSrcLex) { // source folder (folder 1)
            Helpers::strToCStr(&folderSrc, confValue);
        }
        else if (confField == folderDestLex) { // destination folder (folder 2)
            Helpers::strToCStr(&folderDest, confValue);
        }
        else if (confField == totalLogLex) { // total log file name
            Helpers::strToCStr(&totalLogName, confValue);
        }
        else if (confField == waitLex) { // wait interval before repeating iteration
            waitSec = size_t(std::stoi(confValue)); // string to intwaitLex
        }
    }
    config.close();
    checkCriticalFields();
}


void Settings::checkCriticalFields() {
    if (!folderSrc)
        throw std::runtime_error("FOLDER_SRC not found in conig");
    if (!folderDest)
        throw std::runtime_error("FOLDER_DEST not found in config");
}

// getters


char const * const Settings::getPidPath() {
    if (pidPath)
        return pidPath;
    else
        return Settings::defaultPidPath;
}


char const * const Settings::getFolderSrc() {
    return folderSrc;
}


char const * const Settings::getFolderDest() {
    return folderDest;
}

char const * const Settings::getTotalLogName() {
    if (totalLogName)
        return totalLogName;
    else
        return Settings::defaultTotalLog;
}

size_t Settings::getWaitSec() {
    return waitSec;
}
