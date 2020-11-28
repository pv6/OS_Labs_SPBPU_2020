#pragma once
#include <string>
#include <cstdlib>
#include <cstdio>

struct Date {
    int Year, Month, Day;

    std::string toString() {
        return std::string(std::to_string(Year)) + "-" + std::to_string(Month) + "-" + std::to_string(Day);
    }

    char *serialize() {
        char *buf = new char[sizeof(Date)];
        *(int *)buf = Year;
        *(int *)(buf + sizeof(int)) = Month;
        *(int *)(buf + 2 * sizeof(int)) = Day;

        return buf;
    }

    static Date deserialize(const char *buf) {
        return Date{*(int *)buf, *(int *)(buf + sizeof(int)), *(int *)(buf + 2 * sizeof(int))};
    }
};