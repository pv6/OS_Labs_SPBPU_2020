//
// Created by Daria on 11/23/2020.
//

#ifndef WEATHER_HOST_CLIENT_CONN_H
#define WEATHER_HOST_CLIENT_CONN_H

#include <stdlib.h>
#include <string>

class conn {
public:
    bool open(size_t id, bool create);
    bool close();

    bool read(void *buf, size_t count);
    bool write(void *buf, size_t count);

    conn() = default;
    ~conn() = default;

private:
    std::string conn_name_;
    bool is_created_;
    int *file_descr_;
};
#endif //WEATHER_HOST_CLIENT_CONN_H
