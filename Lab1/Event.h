#ifndef EVENT_H
#define EVENT_H
#include <string>
#include <sstream>
#include <time.h>
#include <ctime>
#include <sstream>
#include <iomanip>

class Event
{
public:
    static Event parse_event(const std::string& line);

    bool check_time(const time_t& curr_time, int interval);
    std::string remind(const time_t& curr_time);

private:
    Event(tm time, const std::string& text, const time_t& repeat);

private:
    tm time;
    time_t last_remind;
    std::string text;
    time_t repeat;
};

#endif //EVENT_H