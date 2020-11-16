#include "goatling.h"
#include "../core/game_constants.h"
#include <stdlib.h>
#include <time.h>

Goatling::Goatling() {
    srand(time(NULL));
    _status = Game::aliveCode;
}

int Goatling::responseToWolf() const {
    if (_status == Game::aliveCode)
        return (rand() % Game::aliveNumMax) + 1;
    else
        return (rand() % Game::deadNumMax) + 1;
}

void Goatling::setStatus(int status) {
    if (status != Game::aliveCode && status != Game::deadCode)
        return;
    _status = status;
}
