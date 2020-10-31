//
// Created by yudzhinnsk on 30.10.2020.
//

#ifndef UNTITLED1_NEEDEDINFO_H
#define UNTITLED1_NEEDEDINFO_H
#include <iostream>
class NeededInfo{
public:
    NeededInfo(const std::string &path);
    NeededInfo(std::string &fPath, std::string &cPath, std::string &Pid, unsigned int val);
    void setFolderPath(std::string &path);
    void setIntervalVal(unsigned int val);
    void setPId(std::string &val);
    std::string& retFolderPath();
    unsigned int retIntervalValue();
    std::string& retConfigPath();
    std::string& retPId();
private:
    std::string FolderPath;
    std::string configPath;
    std::string  PId = "/var/run/lab1";
    unsigned int IntervalValue = 0;
};
#endif //UNTITLED1_NEEDEDINFO_H
