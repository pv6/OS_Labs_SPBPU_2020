#include "wolfer.h"
#include "../core/game_constants.h"
#include <time.h>
#include <stdlib.h>

Wolfer::Wolfer() {
    srand(time(NULL));
    _wolferNumber = 0;
}

int Wolfer::processAliveGoatling(int number) {
    if (number > Game::aliveNumMax || number < 1)
        return Game::errorCode;

    if (abs(number - _wolferNumber) <= Game::survivalGap)
        return Game::aliveCode;
    else
        return Game::deadCode;
}

int Wolfer::processDeadGoatling(int number) {
    if (number > Game::deadNumMax || number < 1)
        return Game::errorCode;

    if (abs(number - _wolferNumber) <= Game::ressurectionGap)
        return Game::aliveCode;
    else
        return Game::deadCode;
}

void Wolfer::generateWolferNumber() {
    _wolferNumber = (rand() % Game::wolferNumMax) + 1;
}
