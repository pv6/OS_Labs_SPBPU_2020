//
// Created by aleksandr on 27.10.2020.
//

#ifndef LAB1_PARSER_H
#define LAB1_PARSER_H

#include <string>
#include <map>
#include <fstream>

class Parser
{
public:
    enum ConfigParam
    {
        PERIOD,
        DIR1,
        DIR2
    };

    static std::map<ConfigParam, std::string> parseFile(std::ifstream& configFile);
private:
    static const std::map<std::string, ConfigParam> configMap;
};

#endif //LAB1_PARSER_H
