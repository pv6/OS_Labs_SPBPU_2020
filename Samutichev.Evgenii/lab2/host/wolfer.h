#ifndef WOLF_H_INCLUDED
#define WOLF_H_INCLUDED

class Wolfer {
public:
    Wolfer();

    bool setWolferNumber(int number);
    int processGoatling(int number);
    bool gameOver() const;

private:
    int processAlive(int number);
    int processDead(int number);

    int _wolferNumber;
    int _goatlingStatus;
    int _deadTurns;
    bool _gameOver;

};

#endif // WOLF_H_INCLUDED
