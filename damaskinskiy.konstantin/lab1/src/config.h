#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>

class Config
{
public:
    explicit Config( std::string const &configName );

    void load();
private:
    std::string configName;
    std::vector<std::string> directories;
};

#endif // CONFIG_H
