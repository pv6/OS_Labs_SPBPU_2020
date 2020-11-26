#ifndef CONN_H_INCLUDED
#define CONN_H_INCLUDED

class Conn{
public:
    static Conn* createConn(bool isCreator);
    virtual void openConn() = 0;
    virtual void readConn(char *buf, size_t count) = 0;
    virtual void writeConn(char *buf, size_t count) = 0;
    virtual void closeConn() = 0;
    virtual ~Conn(){}
protected:
    Conn(){}
};

#endif // CONN_H_INCLUDED
