#ifndef CONNHELPER_H_INCLUDED
#define CONNHELPER_H_INCLUDED

#include <string>

class ConnHelper{
public:
    static const std::string SEM_HOST_NAME;
    static const std::string SEM_CLIENT_NAME;
    static const int TIMEOUT = 5;
    static const int TIMEOUT_CLIENT = 60;
    static const int BUF_SIZE = 11;
    static const int MIN_TEMPERATURE = -50;
    static const int MAX_TEMPERATURE = 50;
};


#endif // CONNHELPER_H_INCLUDED
