//
// Created by Daria on 10/22/2020.
//

#ifndef DAEMON_LAB1_PARSER_H
#define DAEMON_LAB1_PARSER_H

#include <map>
#include <string>
#include <set>
#include <fstream>
#include "error.h"

class parser {
public:
    static error::error_name parse_config(std::ifstream& file);
    std::map<std::string, std::string> get_map() {
        return this->config_map;
    }
    static const std::set<std::string> id_file;
private:
    static std::map<std::string, std::string> config_map;
    static const std::string delimiter;
};

#endif //DAEMON_LAB1_PARSER_H
