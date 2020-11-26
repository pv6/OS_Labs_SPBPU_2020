//
// Created by Daria on 11/23/2020.
//

#ifndef WEATHER_HOST_CLIENT_MESSAGE_H
#define WEATHER_HOST_CLIENT_MESSAGE_H

const int timeout = 5;
class message {
public:
    message() = default;
    message(int d, int m, int y) : temp_(0), day_(d), month_(m), year_(y) {};
    message(double t, int d, int m, int y) : temp_(t), day_(d), month_(m), year_(y) {};
    void set_day(int day) { day_ = day; }
    void set_month(int m) { month_ = m; }
    void set_year(int y) { year_ = y; }
    void set_temp(int t) { temp_ = t; }
    int get_day() { return day_; }
    int get_month() { return month_; }
    int get_year() { return year_; }
    int get_temp() { return temp_; }
private:
    int temp_;
    int day_;
    int month_;
    int year_;
};

#endif //WEATHER_HOST_CLIENT_MESSAGE_H
