#ifndef WOLF_H_INCLUDED
#define WOLF_H_INCLUDED

class Wolfer {
public:
    Wolfer();

    int processAliveGoatling(int number);
    int processDeadGoatling(int number);
    void generateWolferNumber();

private:
    int _wolferNumber;
};

#endif // WOLF_H_INCLUDED
