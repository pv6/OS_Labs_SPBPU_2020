#ifndef CONFIGPARSER_H_INCLUDED__
#define CONFIGPARSER_H_INCLUDED__

#include <string>
#include <vector>

class ConfigParser {
    public:
        ConfigParser() = default;
        ConfigParser(std::string config_filename);
        std::vector<std::string> read_config();

    private:
        std::string _config_filename;
};

#endif