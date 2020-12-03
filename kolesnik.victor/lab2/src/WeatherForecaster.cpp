#include "../include/WeatherForecaster.h"

#include <cstdlib>


WeatherForecaster::WeatherForecaster(int id)
    : _id(id)
{}
int WeatherForecaster::forecast(int day, int month, int year) {
    srand(day + month * 32 + year * 12 * 32);
    return rand() + _id;
}