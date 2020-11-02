#include <fstream>
#include <filesystem>
#include <sys/syslog.h>
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

    std::string eventstr = "MaxEvents";
    std::string s;
    ifs >> s;
    if (s == eventstr)
    {
        size_t mevts;
        ifs >> mevts;
        if (mevts <= 0)
            syslog(LOG_ERR, "Negate maxEvents. Use default");
        else
            maxEvents = mevts;
    }
    else
        syslog(LOG_ERR, "Use default maxEvents: %li", maxEvents);

    std::string dir;

    while (ifs)
    {
        ifs >> dir;
        if (!dir.empty() && dir[0] != '#')
        {
            auto abspath = std::filesystem::absolute(dir);
            if (std::filesystem::exists(abspath))
            {
                syslog(LOG_INFO, "Push directory: %s", abspath.c_str());
                directories.push_back(abspath.string());
            }
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

size_t Config::getMaxEvents() const
{
    return maxEvents;
}
