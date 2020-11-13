#ifndef GAME_CONSTANTS_H_INCLUDED
#define GAME_CONSTANTS_H_INCLUDED

namespace Game {
    const int aliveCode = 0;
    const int deadCode = 1;
    const int errorCode = 2;
    const int wolferNumMax = 100;
    const int aliveNumMax = 100;
    const int deadNumMax = 50;
    const int survivalGap = 70; // because n = 1
    const int ressurectionGap = 20; // because n = 1
    const int turnsToGameOver = 2;
}

#endif // GAME_CONSTANTS_H_INCLUDED
