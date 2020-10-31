#include "../include/ConfigParser.h"

#include <fstream>
#include <iostream>

ConfigParser::ConfigParser(std::string config_filename)
    : _config_filename(config_filename)
{}
std::vector<std::string> ConfigParser::read_config() {
    std::ifstream config(_config_filename);
    std::vector<std::string> values;

    if (config.is_open()) {
        std::string dir1_token = "DIR_1=";
        std::string dir2_token = "DIR_2=";
        std::string time_token = "WAIT_TIME=";

        std::string dir1_bufer;
        std::string dir2_bufer;
        std::string time_bufer;

        std::string line;

        std::getline(config, line);
        if (line.rfind(dir1_token, 0) == 0) {
            dir1_bufer = line.substr(dir1_token.length(), line.length() - dir1_token.length());
        } else {
            config.close();
            return values;
        }
        std::getline(config, line);
        if (line.rfind(dir2_token, 0) == 0) {
            dir2_bufer = line.substr(dir2_token.length(), line.length() - dir2_token.length());
        } else {
            config.close();
            return values;
        }
        std::getline(config, line);
        if (line.rfind(time_token, 0) == 0) {
            time_bufer = line.substr(time_token.length(), line.length() - time_token.length());
        } else {
            config.close();
            return values;
        }

        values.push_back(dir1_bufer);
        values.push_back(dir2_bufer);
        values.push_back(time_bufer);
        
        config.close();

        return values;
    } else {
        return values;
    }
}