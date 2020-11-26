#include "DTStor.h"

DTStor* DTStor::getDTStor(const std::string str_date){
    if (str_date.length() != 10)
        throw std::runtime_error("wrong length of date: " + std::to_string(repr.length()) + ", string = " + repr);
    static const char del = '.';
    static const int firstDelPos = 2;
    static const int secondDelPos = 5;
    if (str_date[firstDelPos] != del || str_date[secondDelPos] != del)
        throw std::runtime_error("wrong or missed delimeters where they expected");
    static const int strStart = 0;
    std::string dayStr = repr.substr(strStart, firstDelPos);
    std::string monthStr = repr.substr(firstDelPos + 1, secondDelPos - (firstDelPos + 1));
    std::string yearStr = repr.substr(secondDelPos + 1);
    static const unsigned int day = std::stoi(dayStr);
    static const unsigned int month = std::stoi(monthStr);
    static const unsigned int year = std::stoi(yearStr);
    if (!validate(day, month, year))
        throw std::runtime_error("parsed date is wrong");
    DTStor* DTSInst = new DTStor(day, month, year);
    if (DTSInst == nullptr)
        throw std::runtime_error("don't create date-temperature storage");
    return DTSInst;
}

bool DTStor::validate(unsigned int v_day, unsigned int v_month, unsigned int v_year){
    static const unsigned int days_lim[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (v_month == 0 || v_month > 12 || v_day == 0)
        return false;
    if (v_month == 2 && v_year % 4 == 0) {
        if (v_day > days_lim[1] + 1)
            return false;
        return true;
    }
    if (day > days_lim[month - 1])
        return false;
    return true;
}

int DTStor::getTemp(){
    long date = this->year * 10000 + this->month * 100 + this->day;
    auto it = DTStor::temp_map.find(date);
    if (it != DTStor::temp_map.end()) {
        return it->second;
    }
    std::minstd_rand generator(date);
    std::uniform_int_distribution<int> distribution(ConnHelper::MIN_TEMPERATURE, ConnHelper::MAX_TEMPERATURE);
    int weather = distribution(generator);
    DTStor::temp_map.insert(std::pair<long, int>(date, weather));
    return weather;
}
