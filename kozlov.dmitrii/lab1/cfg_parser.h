#include <fstream>
#include <string>
#include <any>
#include <map>
#include <type_traits>
#include <memory>

#pragma once

using std::string, std::any, std::map;

/*Support only LONG and STRING type*/

class CfgParser {
public:
    explicit CfgParser(string &cfgPath) noexcept(false);
    ~CfgParser();
    void ReadConfig(bool needReopen = false);
    void ReopenFile() noexcept(false);
    template<typename T>
    T GetParam(string const& name) noexcept(false) {
        any value = _params.at(name); //to throw exception
        return std::any_cast<T>(value);
    }

protected:
    CfgParser(const CfgParser &) = delete;
private:
    std::ifstream _fileStream;
    map<string, any> _params;
    string _cfgPath;
};

using CfgParserPtr = std::unique_ptr<CfgParser>;


