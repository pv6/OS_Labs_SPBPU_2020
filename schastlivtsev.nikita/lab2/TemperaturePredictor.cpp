#include "TemperaturePredictor.h"
#include <iostream>

TemperaturePredictor::TemperaturePredictor(int seed): seed(seed)
{
    //ctor
}

TemperaturePredictor::~TemperaturePredictor()
{
    //dtor
}

int TemperaturePredictor::predict(const DateHolder& date) {
    size_t day = date.getDay();
    size_t month = date.getMonth();
    size_t year = date.getYear();

    // logic: the bigger year -- the higher temperature
    // winter months -> cold; summer months -> warm
    // days ->  base + rand(from=0, to=3) Celsius degrees

    float trendMult = calculateTrend(year); // year-dependent
    float base = calculateBase(month);
    float noize = deterministicRandom(0, 3, day + 31 * month);

    return int(base * trendMult + noize);
}

float TemperaturePredictor::calculateTrend(const size_t year) const {
    // year-dependent trend
    static const float trendA0 = 0.6f;
    static const float trendA1 = 0.0001f;
    return float(trendA0 + year * trendA1);
}

float TemperaturePredictor::calculateBase(const size_t month) const {
    // 0 - winter => -8 degrees, 1 - spring/fall => 8 degrees, 2 - summer => 16 degrees
    static const float seasonBase[] = {-8, 8, 16}; // array of base temperatures
    // season idxs
    static const int winterIdx = 0;
    static const int springFallIdx = 1;
    static const int summerIdx = 2;

    // season borders
    static const int feb = 1;
    static const int dec = 11;
    static const int june = 5;
    static const int august = 7;

    // determine season
    int seasonIdx;
    if (month <= feb || month >= dec)
        seasonIdx = winterIdx;
    else if (month >= june && month <= august)
        seasonIdx = summerIdx;
    else
        seasonIdx = springFallIdx;

    return seasonBase[seasonIdx];
}

float TemperaturePredictor::deterministicRandom(const float limitMin, const float limitMax, const size_t key) {
    if (limitMin >= limitMax)
        throw std::runtime_error("limitMin was higher than limitMax in TemperaturePredictor::deterministicRandom");
    std::srand(seed); // set seed to be deterministic
    int randVar;
    for (size_t i = 0; i < key; ++i) // use cycle to get not the first random variable
        randVar = std::rand();
    randVar = std::rand(); // use once more as key can be 0
    // random number from limitMin to limitMax
    double ansDouble = (double(limitMax) - limitMin) * (double(randVar) / (double(RAND_MAX) + 1)) + double(limitMin);
    return float(ansDouble); // cast to float
}
