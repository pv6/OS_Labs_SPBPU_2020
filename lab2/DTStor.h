#ifndef DTSTOR_H_INCLUDED
#define DTSTOR_H_INCLUDED

#include <string>
#include <map>
#include "ConnHelper.h"
#include <stdexcept>
#include <random>
#include <cstdlib>


class DTStor{
public:
    unsigned int getDay(){return this->day;}
    unsigned int getMonth(){return this->month;}
    unsigned int getYear(){return this->year;}
    int getTemp();
    static DTStor* getDTStor(const std::string &str_date);
    DTStor(unsigned int day = 0, unsigned int month = 0, unsigned int year = 0) :
            day{day}, month{month}, year{year} {};
private:
    static bool validate(unsigned int v_day, unsigned int v_month, unsigned int v_year);
    static std::map<long, int> temp_map;
    unsigned int day;
    unsigned int month;
    unsigned int year;
};

#endif // DTSTOR_H_INCLUDED
