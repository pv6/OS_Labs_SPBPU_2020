#ifndef CONN_H_INCLUDED
#define CONN_H_INCLUDED

#include <cstdio>

class Conn {
public:
        virtual void Read(char* buf, const size_t len) = 0;
        virtual void Write(const char* buf, const size_t len) = 0;
        virtual ~Conn();
        static Conn* createConnection(bool isHost);
protected:
	Conn();
};

#endif // CONN_H_INCLUDED
