#ifndef GOATLING_H_INCLUDED
#define GOATLING_H_INCLUDED

class Goatling {
public:
    Goatling();
    int responseToWolf() const;
    void setStatus(int status);

private:
    int _status;
};

#endif // GOATLING_H_INCLUDED
