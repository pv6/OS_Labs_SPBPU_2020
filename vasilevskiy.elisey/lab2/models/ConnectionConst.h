#ifndef LAB1_CONNECTIONCONST_H
#define LAB1_CONNECTIONCONST_H


#include <string>

class ConnectionConst {
public:
    static const std::string SEM_HOST_NAME;
    static const std::string SEM_CLIENT_NAME;
    static const int TIMEOUT = 5;
    static const int MIN_TEMPERATURE = -50;
    static const int MAX_TEMPERATURE = 50;
};
#endif //LAB1_CONNECTIONCONST_H
