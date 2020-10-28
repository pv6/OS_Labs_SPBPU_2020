
#include "parser.h"
#include <vector>

const map<string, parser::ParserConfig> parser::cnfName = {
        {"Time", parser::TIME},
        {"Dir1", parser::DIR1},
        {"Dir2", parser::DIR2}
};

configData parser::parseConfigFile(const string& configFile) {
    configData configDataParse;
    configDataParse.time = -1;
    configDataParse.dir1 = "";
    configDataParse.dir2 = "";
    ifstream file(configFile);
    string str;

    if (!file.is_open()) {
        throw runtime_error("Config file don't open");
    }
    while (getline(file, str)){
    //while (!file.eof()) {
      //  getline(file, str);
        vector<string> data;
        string strConfig;
        for (char c : str) {
            if (!isspace(c) && c != separator) {
                strConfig.push_back(c);
            }
            if (c == separator) {
                if (!strConfig.empty())
                    data.push_back(strConfig);
                strConfig.clear();
            }
            if (isspace(c)) {
                if (!strConfig.empty())
                    data.push_back(strConfig);
                strConfig.clear();
            }
        }
        if (!strConfig.empty())
            data.push_back(strConfig);
        strConfig.clear();
        if (data.size() != 2) {
            throw runtime_error("Arguments don't equal 2");
        }
        if (cnfName.find(data[0]) != cnfName.end()) {
            int time;
            switch (cnfName.at(data[0])) {
                case parser::TIME:
                    try {
                        time = stoi(data[1]);
                    }
                    catch (exception & e) {
                        throw e;
                    }
                    if (time < 0) {
                        throw runtime_error("Time < 0");
                    }
                    configDataParse.time = time;
                    break;
                case parser::DIR1:
                    configDataParse.dir1 = data[1];
                    break;
                case parser::DIR2:
                    configDataParse.dir2 = data[1];
                    break;
            }
        }
        else {
            throw runtime_error("Wrong config parse name");
        }
    }
    file.close();
    return configDataParse;
}