#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include <map>
#include <fstream>

class Parser {
public:
    enum Param {
        INTVAL,
        IN_DIR,
        OUT_DIR,
        EXTENSION
    };

    static std::map<Parser::Param, std::string> parseConfig(const std::string &confingFile);

private:
    static const int configSize;
    static const std::string separator;
    static const std::string intval;
    static const std::string input;
    static const std::string output;
    static const std::string extension;
};

#endif //PARSER_H_