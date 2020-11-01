//
// Created by yudzhinnsk on 30.10.2020.
//
#include <iostream>
#include <string>
#include "NeededInfo.h"
void NeededInfo::setFolderPath(std::string &path){
    this->FolderPath = path;
}
void NeededInfo::setIntervalVal(unsigned int val){
    this->IntervalValue = val;
}
void NeededInfo::setPId(std::string& val){
    this->PId = val;
}
std::string &NeededInfo::retFolderPath(){
    return FolderPath;
}
unsigned int NeededInfo::retIntervalValue(){
    return IntervalValue;
}
std::string &NeededInfo::retConfigPath(){
    return configPath;
}
std::string &NeededInfo::retPId(){
    return PId;
}
NeededInfo::NeededInfo(const std::string &path){
    this->configPath = path;
}
NeededInfo::NeededInfo(std::string &fPath, std::string &cPath, std::string &Pid, unsigned int val) {
    this->FolderPath = fPath;
    this->configPath = cPath;
    this->PId = PId;
    this->IntervalValue = val;
}

