#include "DTO.h"
#include "ConnectionConst.h"

void DTO::setTemp(int temperature) {
    this->temperature = temperature;
}

int DTO::getTemp() const {
    return temperature;
}

unsigned int DTO::getDay() const {
    return day;
}

unsigned int DTO::getMonth() const {
    return month;
}

unsigned int DTO::getYear() const {
    return year;
}

