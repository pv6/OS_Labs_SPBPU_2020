#pragma once
#include <string>
#include <cstdlib>
#include <cstdio>

struct Date {
    int Year, Month, Day;

    std::string toString() {
        return std::string(std::to_string(Year)) + "-" + std::to_string(Month) + "-" + std::to_string(Day);
    }
};