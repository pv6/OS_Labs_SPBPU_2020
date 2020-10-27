//
// Created by aleksandr on 27.10.2020.
//

#include "parser.h"

std::map<std::string, Parser::ConfigParam> const Parser::configMap = {
        {"period", PERIOD},
        {"dir1", DIR1},
        {"dir2", DIR2},
};

std::map<Parser::ConfigParam, std::string> Parser::parseFile(std::ifstream &configFile) {
    std::map<ConfigParam, std::string> config;
    for(int i=0; i<configMap.size(); i++) {
        std::string parameter;
        std::string value;

        try {
            configFile >> parameter;
            configFile >> value;

            config.insert({configMap.at(parameter), value});

        }
        catch (...) {
            throw std::runtime_error("Couldn't parse line " + std::to_string(i));
        }

    }

    return config;
}
