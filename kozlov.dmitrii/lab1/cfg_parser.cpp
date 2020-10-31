#include "cfg_parser.h"

void CfgParser::ReadConfig(bool needReopen) {
    if (needReopen)
        ReopenFile();
    _params.clear();
    string param, value;
    while (_fileStream >> param >> value){
        try { //try to cast to number
            long isNum = std::stol(value);
            _params[param] = isNum;
        }
        catch (const std::invalid_argument & ex) {
            _params[param] = value;
        }

    }
}

void CfgParser::ReopenFile() noexcept(false)
{
    _fileStream.clear();
    _fileStream.close();
    _fileStream.open(_cfgPath, std::ios::binary);
    if (!_fileStream.is_open())
        throw std::ifstream::failure("Can't open cfg file");
}

CfgParser::CfgParser(string &cfgPath) : _cfgPath(cfgPath) {
    _fileStream.open(cfgPath, std::ios::binary);
    if (!_fileStream.is_open())
        throw std::ifstream::failure("Can't open cfg file");
}

CfgParser::~CfgParser() {
    _fileStream.close();
}
