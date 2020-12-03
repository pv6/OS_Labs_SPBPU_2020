#ifndef WEATHERFORECASTER_H_INCLUDED__
#define WEATHERFORECASTER_H_INCLUDED__

class WeatherForecaster {
public:
    WeatherForecaster(int id);

    int forecast(int day, int month, int year);

private:
    int _id;
};

#endif