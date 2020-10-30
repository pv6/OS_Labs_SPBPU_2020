#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>

class Config
{
public:
    Config() {}
    void setName( std::string const &configName );
    Config( std::string const &configName );

    void load();
    std::vector<std::string> const& getDirectories() const;
private:
    std::string configName;
    std::vector<std::string> directories;
};

#endif // CONFIG_H
