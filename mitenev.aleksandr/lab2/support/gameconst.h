//
// Created by aleksandr on 26.11.2020.
//

#ifndef LAB2_GAMECONST_H
#define LAB2_GAMECONST_H

#include <string>


class GameConst {
public:
    static const std::string SEM_HOST_NAME;
    static const std::string SEM_CLIENT_NAME;
    static const int TIMEOUT = 10;
    static const int MAX_ALIVE_NUM = 100;
    static const int MAX_WOLF_NUM = 100;
    static const int MAX_DEAD_NUM = 50;
    static const int MAX_DEATHS = 2;
};

const std::string GameConst::SEM_HOST_NAME="HOST_LAB2";
const std::string GameConst::SEM_CLIENT_NAME="CLIENT_LAB2";

#endif //LAB2_GAMECONST_H
