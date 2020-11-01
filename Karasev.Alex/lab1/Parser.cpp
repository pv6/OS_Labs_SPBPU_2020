#include <fstream>
#include "Parser.h"

std::vector<std::string> Parser::parse(const std::string& configFileName) {
    std::string folder_1, folder_2;
    std::ifstream configFile(configFileName);
    std::vector<std::string> folders;
    if (!configFile.is_open()) {
        syslog(LOG_ERR, "Couldn't open config file");
    }

    configFile >> folder_1 >> folder_2;
    folders.push_back(folder_1);
    folders.push_back(folder_2);
    configFile.close();
    return folders;
}