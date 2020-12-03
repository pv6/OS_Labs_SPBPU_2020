#include "DTStor.h"
#include <iostream>

unsigned int DTStor::day = 0;
unsigned int DTStor::month = 0;
unsigned int DTStor::year = 0;

void DTStor::parseToDTStor(const std::string &str_date){
    if (str_date.length() != 10)
        throw std::runtime_error("wrong length of date: " + std::to_string(str_date.length()) + ", string = " + str_date);
    std::cout << "str_date: " << str_date << std::endl;
    static const char del = '.';
    static const int firstDelPos = 2;
    static const int secondDelPos = 5;
    if (str_date[firstDelPos] != del || str_date[secondDelPos] != del)
        throw std::runtime_error("wrong or missed delimeters where they expected");
    static const int strStart = 0;
    std::string dayStr = str_date.substr(strStart, firstDelPos);
    std::string monthStr = str_date.substr(firstDelPos + 1, secondDelPos - (firstDelPos + 1));
    std::string yearStr = str_date.substr(secondDelPos + 1);
    DTStor::day = std::stoi(dayStr);
    DTStor::month = std::stoi(monthStr);
    DTStor::year = std::stoi(yearStr);
    if (!DTStor::validate())
        throw std::runtime_error("parsed date is wrong");
    std::cout << day << month << year << std::endl;
}

bool DTStor::validate(){
    static const unsigned int days_lim[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 0 || month > 12 || day == 0)
        return false;
    if (month == 2 && year % 4 == 0) {
        if (day > days_lim[1] + 1)
            return false;
        return true;
    }
    if (day > days_lim[month - 1])
        return false;
    return true;
}

int DTStor::getTemp(){
    long date = year * 10000 + month * 100 + day;
    std::minstd_rand generator(date);
    std::uniform_int_distribution<int> distribution(ConnHelper::MIN_TEMPERATURE, ConnHelper::MAX_TEMPERATURE);
    int temperature = distribution(generator);
    std::cout << "For date: " << date << " temperature: " << temperature << std::endl;
    return temperature;
}
