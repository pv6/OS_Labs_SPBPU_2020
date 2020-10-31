#include <fstream>
#include "parser.h"

const int Parser::configSize = 4;
const std::string Parser::separator = "=";
const std::string Parser::intval = "intval";
const std::string Parser::input = "in_dir";
const std::string Parser::output = "out_dir";
const std::string Parser::extension = "extension";

void Parser::putIfNew(Parser::Param param, const std::string &name, const std::string &value,
                      std::map<Parser::Param, std::string> &result) {
    if (result.find(param) != result.end()) {
        throw std::runtime_error("Duplicate config name:" + name + value);
    }
    result.insert(std::make_pair(param, value));
}

std::map<Parser::Param, std::string> Parser::parseConfig(const std::string &confingFile) {
    std::ifstream in(confingFile);
    if (!in) {
        throw std::runtime_error("Can't open config file");
    }
    std::map<Parser::Param, std::string> result;
    std::string current;
    while (!in.eof()) {
        std::getline(in, current);
        if (!current.empty()) {
            int separatorPos = current.find(separator);
            if (separatorPos + 1 >= current.size() || separatorPos < 0) {
                throw std::runtime_error("Cannot find separator");
            }
            std::string paramName = current.substr(0, separatorPos);
            std::string paramValue = current.substr(separatorPos + 1);
            if (paramName == intval) {
                putIfNew(Param::INTVAL, paramName, paramValue, result);
            } else if (paramName == input) {
                putIfNew(Param::IN_DIR, paramName, paramValue, result);
            } else if (paramName == output) {
                putIfNew(Param::OUT_DIR, paramName, paramValue, result);
            } else if (paramName == extension) {
                putIfNew(Param::EXTENSION, paramName, paramValue, result);
            } else {
                throw std::runtime_error("Wrong config name :" + paramName);
            }
        }
    }
    in.close();
    if (result.size() != configSize) {
        throw std::runtime_error("Not enough params in config");
    }
    return result;
}