#ifndef TEMPERATUREPREDICTOR_H
#define TEMPERATUREPREDICTOR_H

#include "DateHolder.h"

class TemperaturePredictor
{
    public:
        TemperaturePredictor(int seed = 0);
        virtual ~TemperaturePredictor();

        int predict(const DateHolder& date);
    protected:

    private:
        int seed;

        // functions
        // random seed is "seed" variable, key is the parameter
        float deterministicRandom(const float limitMin, const float limitMax, const size_t key);
        float calculateTrend(const size_t year) const;
        float calculateBase(const size_t month) const;
};

#endif // TEMPERATUREPREDICTOR_H
