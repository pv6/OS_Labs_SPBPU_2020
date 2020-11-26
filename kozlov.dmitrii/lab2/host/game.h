#pragma once

namespace GAME_CONSTANTS {
    const int TIMEOUT = 5;
    const int SEM_RECONNECT_TIMEOUT = 5;
    const int MAX_GOAT_VAL_ALIVE = 100;
    const int MAX_GOAT_VAL_DEAD = 50;
    const int MIN_GOAT_VAL_ALIVE = 1;
    const int MIN_GOAT_VAL_DEAD = 1;
    const int MAX_WOLF_VAL = 100;
    const int MIN_WOLF_VAL = 1;
    const int ALIVE_DEVIATION = 70;
    const int DEAD_DEVIATION = 20;
}

enum class GOAT_STATE
{
    ALIVE = 0,
    HALF_DEAD,
    DEAD
};
