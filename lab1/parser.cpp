//
// Created by Daria on 10/22/2020.
//
#include "parser.h"
#include <algorithm>

void delete_space(std::string& str) {
    for (size_t i = 0; i < str.length(); i++) {
        if (isspace(str.at(i))) {
            str.erase(i, 1);
            i--;
        }
    }
}

const std::string parser::delimiter = "=";
const std::set<std::string> parser::id_file = {"dir_1", "dir_2", "interval"};
std::map<std::string, std::string> parser::config_map;

error::error_name parser::parse_config(std::ifstream& file) {
    std::string current_line;
    while (!file.eof()) {
        std::getline(file, current_line);
        std::size_t pos = current_line.find(delimiter);
        if (pos != std::string::npos) {
            std::string first_part = current_line.substr(0, pos);
            std::string second_part = current_line.substr(pos + 1, current_line.length() - pos);
            delete_space(first_part);
            delete_space(second_part);
            std::set<std::string>::iterator it;
            it = id_file.find(first_part);
            if (it != id_file.end()) {
                config_map.insert(std::pair<std::string, std::string>(first_part, second_part));
            } else {
                return error::WRONG_CONFIG;
            }
        } else {
            return error::WRONG_CONFIG;
        }
    }
    return error::OK;
}

