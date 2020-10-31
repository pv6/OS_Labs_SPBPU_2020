//
// Created by yudzhinnsk on 30.10.2020.
//
#ifndef UNTITLED1_SUBMETHODS_H
#define UNTITLED1_SUBMETHODS_H
#include <iostream>
#include <string>
#include "NeededInfo.h"
class SubMethods{
public:
    static bool is_dir_exist(std::string& path);
    static bool is_file_exist(const std::string &name);
    static int read_config_file(NeededInfo &nf);
};
#endif //UNTITLED1_SUBMETHODS_H
