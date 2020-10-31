#include "Event.h"

#include <iostream>

Event Event::parse_event(const std::string& line) {
    tm time;
    time_t repeat = 0;
    std::string text;

    std::stringstream ss(line);
    std::string token;

    ss >> std::get_time(&time, "%d.%m.%Y %H:%M");
    ss >> token;

    if (token == "-w")
        repeat = 604800;
    else if (token == "-d")
        repeat = 86400;
    else if (token == "-h")
        repeat = 3600;
    else
        text += token;

    while (!ss.eof()) {
        ss >> token;
        if (!text.empty())
            text += " ";
        text += token;
    }
    return Event(time, text, repeat);
}

bool Event::check_time(const time_t& curr_time, int interval) {
    if (repeat == 0) {
        time_t remain = curr_time - std::mktime(&time);
        if (0 < remain && remain < interval) return true;
    }
    else {
        time_t start_time = std::mktime(&time);
        time_t remain = (curr_time - start_time) - (((curr_time - start_time) / repeat) * repeat);
        if (0 < remain && remain < interval) return true;
    }

    return false;
}

std::string Event::remind(const time_t& curr_time) {
    last_remind = curr_time;

    return text;
}

Event::Event(tm time, const std::string& text, const time_t& repeat) {
    this->time = time;
    this->text = text;
    this->repeat = repeat;
    last_remind = std::mktime(&time);
}