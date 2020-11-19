#include "wolfer.h"
#include "../core/game_constants.h"
#include <time.h>
#include <stdlib.h>

Wolfer::Wolfer() {
    _wolferNumber = 0;
    _goatlingStatus = Game::aliveCode;
    _gameOver = false;
    _deadTurns = 0;
}

int Wolfer::processGoatling(int number) {
    if (_goatlingStatus == Game::aliveCode)
        _goatlingStatus = processAlive(number);
    else
        _goatlingStatus = processDead(number);

    if (_deadTurns > Game::turnsToGameOver && _goatlingStatus != Game::errorCode)
        _gameOver = true;

    return _goatlingStatus;
}

bool Wolfer::gameOver() const {
    return _gameOver;
}

int Wolfer::processAlive(int number) {
    if (number > Game::aliveNumMax || number < 1)
        return Game::errorCode;

    if (abs(number - _wolferNumber) <= Game::survivalGap)
        return Game::aliveCode;
    else
        return Game::deadCode;
}

int Wolfer::processDead(int number) {
    if (number > Game::deadNumMax || number < 1)
        return Game::errorCode;

    if (abs(number - _wolferNumber) <= Game::ressurectionGap) {
        _deadTurns = 0;
        return Game::aliveCode;
    }
    else {
        _deadTurns++;
        return Game::deadCode;
    }
}

bool Wolfer::setWolferNumber(int number) {
    if (number > Game::wolferNumMax || number < 1)
        return false;
    _wolferNumber = number;
    return true;
}
