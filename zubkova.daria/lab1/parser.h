
#ifndef LAB1_PARSER_H
#define LAB1_PARSER_H


#include <map>
#include <fstream>

using namespace std;

typedef struct configData {
    int time;
    string dir1;
    string dir2;
} configData;

class parser {
public:
    enum ParserConfig
    {
        TIME,
        DIR1,
        DIR2
    };
    static configData parseConfigFile(const string& configFile);
private:
    static const char separator = '=';
    static const map<string, ParserConfig> cnfName;
};

#endif //LAB1_PARSER_H
