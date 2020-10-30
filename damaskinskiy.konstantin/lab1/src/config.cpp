#include <fstream>
#include <filesystem>
#include "config.h"

Config::Config( std::string const &configName ) :
    configName(configName)
{}

void Config::setName(const std::string &configName) { this->configName = configName;}

void Config::load()
{
    std::ifstream ifs(configName);

    if (!ifs)
        throw std::invalid_argument("Config file " + configName + "does not exist");

    std::string dir;
    while (ifs)
    {
        ifs >> dir;
        if (!dir.empty() && dir[0] != '#')
        {
            auto abspath = std::filesystem::absolute(dir);
            if (std::filesystem::exists(abspath))
                directories.push_back(abspath.string());
            else
                throw std::runtime_error("Directory " + abspath.string() + " does not exist\n");
        }
    }
    ifs.close();
}

std::vector<std::string> const& Config::getDirectories() const
{
    return directories;
}
