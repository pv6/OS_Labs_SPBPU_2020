#include "DateHolder.h"
#include <iostream>  // for runtime errors

DateHolder::DateHolder(const size_t day, const size_t month, const size_t year): day(day), month(month), year(year)
{
    //ctor
    // fields are initialized but we have to validate arguments
    // if the arguments are wrong, exception will be raised
    validateDMY(day, month, year);
}

DateHolder::~DateHolder()
{
    //dtor
}

DateHolder::DateHolder(const std::string& repr) {
    // date format is "dd.mm.yyyy", no spaces, fixed numbers length

    // validate length
    if (repr.length() != 10)
        throw std::runtime_error("Can't parse date: wrong length: " + std::to_string(repr.length()) + ", string = " + repr);
    static const char delimeter = '.';

    // detect delimeters on the right positions
    static const int firstDelimeterPos = 2;
    static const int secondDelimeterPos = 5;
    if (repr[firstDelimeterPos] != delimeter || repr[secondDelimeterPos] != delimeter)
        throw std::runtime_error("Can't parse date: wrong or missed delimeters where they expected");

    // derive string and parse day, month and year
    static const int strStart = 0;
    std::string dayStr = repr.substr(strStart, firstDelimeterPos);
    std::string monthStr = repr.substr(firstDelimeterPos + 1, secondDelimeterPos - (firstDelimeterPos + 1));
    std::string yearStr = repr.substr(secondDelimeterPos + 1);

    // strings to int fields
    day = std::stoi(dayStr);
    month = std::stoi(monthStr);
    year = std::stoi(yearStr);

    // validate fields
    validateDMY(day, month, year);
}

void DateHolder::validateDMY(const size_t day, const size_t month, const size_t year) {
    static const int days_lim[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 0)
	throw std::runtime_error("Can't create date: month was 0");
    if (month > 12)
        throw std::runtime_error("Can't create date: too big month value");
    if (month == 1 && year % 4 == 0) {
        // leap year, february
        if (day > size_t(days_lim[1]) + 1)
            throw std::runtime_error("Can't create date: too big day value for month");
        return; // valid date, success
    }
    if (day > size_t(days_lim[month - 1]))  // index from 0 to 11, month from 1 to 12
        throw std::runtime_error("Can't create date: too big day value for month");
    // if we reach here, the date is valid
}

std::string DateHolder::serialize() {
    // the format is "dd.mm.yyyy"
    std::string dayStr = std::to_string(day);
    std::string monthStr = std::to_string(month);
    std::string yearStr = std::to_string(year);

    // check if we need to add extra zeroes to meet format
    if (day < 10) // dayStr must consist of 2 numbers
        dayStr = "0" + dayStr;
    if (month < 10) // monthStr must consist of 2 numbers
        monthStr = "0" + monthStr;
    if (year < 1000) { // yearStr must consist of 4 numbers
        if (year < 100) {
            if (year < 10)
                yearStr += "000";
            else
                yearStr += "00";
	} else
            yearStr += "0";
    }
    // create final string
    static const char delimeter = '.';
    return dayStr + delimeter + monthStr + delimeter + yearStr;
}

size_t DateHolder::getDay() const {
    return day;
}

size_t DateHolder::getMonth() const {
    return month;
}

size_t DateHolder::getYear() const {
    return year;
}
