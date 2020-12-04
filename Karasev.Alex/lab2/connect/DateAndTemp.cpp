//
// Created by alex on 25.11.2020.
//

#include "DateAndTemp.h"

int DateAndTemp::getTemp() const {
    return temp;
}

void DateAndTemp::setTemp(int temperature) {
    temp = temperature;
}

unsigned int DateAndTemp::getDay() const {
    return day;
}

unsigned int DateAndTemp::getMonth() const {
    return month;
}

unsigned int DateAndTemp::getYear() const {
    return year;
}
