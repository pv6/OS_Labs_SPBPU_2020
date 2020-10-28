#include "LogLogger.h"
#include "Helpers.h"
#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdio>
#include <syslog.h>

LogLogger::LogLogger()
{
    //ctor
}

LogLogger::~LogLogger()
{
    //dtor
    if (srcDir) {
        delete [] srcDir;
        srcDir = nullptr;
    }
    if (destDir) {
        delete [] destDir;
    }
    if (totalLogName) {
        delete [] totalLogName;
    }
}

// setters

void LogLogger::setDirs(char const * const src, char const * const dest) {
    // TODO: implement
    if (!src || !dest)
        throw std::runtime_error("Trying to assign nullptr as src or dest files");
    Helpers::copyCStr(&srcDir, src);
    Helpers::copyCStr(&destDir, dest);
}


void LogLogger::setTotalLogName(char const * const filename) {
    if (!filename)
        throw std::runtime_error("Trying to assign nullptr as total log file name");
    Helpers::copyCStr(&totalLogName, filename);
}


// main procedure
void LogLogger::run() {
    if (!(readyLog()))
        throw std::runtime_error("Can't log: src and dest dirs haven't been set");
    std::ifstream infile;
    std::ofstream outfile;
    outfile.open(getTotalLogPath(), std::ios::out | std::ios::app);
    try {  // If exception occurs here, we have to close files
        std::string fileContent;
        std::string currentFile;
        namespace fs = std::filesystem;
        for (const auto & entry : fs::directory_iterator(srcDir)) {
            std::stringstream fileStream;
            currentFile = entry.path();
            infile.open(currentFile); // open current file
            fileStream << infile.rdbuf(); // read file
            infile.close(); // have to close the file
            fileContent = fileStream.str(); // make a string to write
            outfile.write("\n\n", 2);  // 2 empty lines
            outfile.write(currentFile.c_str(), currentFile.length()); // current file name
            outfile.write("\n\n", 2); // finish file name with \n and add 1 empty line
            outfile.write(fileContent.c_str(), fileContent.length()); // log content
            std::remove(currentFile.c_str()); // delete file as it has been already logged
        }
    } catch(std::exception& ex) {
        if (infile.is_open())
            infile.close();
        outfile.close();
        throw std::runtime_error(ex.what());
    }
    outfile.close();
    syslog(LOG_INFO, "Logger iteration performed");
}


// private

bool LogLogger::readyLog() {
    return !!(srcDir && destDir);
}

std::string LogLogger::getTotalLogPath() {
    std::string ans(destDir);
    if (totalLogName)
        ans += std::string("/") + totalLogName;
    else
        ans += std::string("/") + totalLogDefault;
    return ans;
}

