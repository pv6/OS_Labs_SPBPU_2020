#ifndef LAB1_PARSER_H
#define LAB1_PARSER_H

#include<vector>
#include<string>
#include<iostream>
#include<syslog.h>

class Parser {
public:
    static std::vector<std::string> parse(const std::string& configFileName);
};

#endif //LAB1_PARSER_H