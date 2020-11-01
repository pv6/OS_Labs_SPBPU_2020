#ifndef UNTITLED_EVENT_H
#define UNTITLED_EVENT_H

#include <iostream>
#include <utility>

class Event {
private:
    std::string eventText_;
    std::string eventFlag_;
    tm eventTime_;
public:
    Event(std::string eventText, std::string eventFlag, tm eventTime) : eventText_(std::move(eventText)),
                                                                        eventFlag_(std::move(eventFlag)),
                                                                        eventTime_(eventTime) {};

    std::string getEventText() { return eventText_; }

    std::string getEventFlag() { return eventFlag_; }

    tm getEventTime() { return eventTime_; }
};


#endif //EVENT_H
