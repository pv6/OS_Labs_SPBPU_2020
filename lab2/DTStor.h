#ifndef DTSTOR_H_INCLUDED
#define DTSTOR_H_INCLUDED

#include <string>
#include <map>
#include "ConnHelper.h"
#include <stdexcept>
#include <random>
#include <cstdlib>
#include <ctime>


class DTStor{
public:
    static int getTemp();
    static void parseToDTStor(const std::string &str_date);
private:
    static bool validate();
    static unsigned int day;
    static unsigned int month;
    static unsigned int year;
    DTStor();
};

#endif // DTSTOR_H_INCLUDED
