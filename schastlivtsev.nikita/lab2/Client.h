#ifndef CLIENT_H
#define CLIENT_H

#include <cstdio>
#include "SemWrapper.h"
#include "Conn.h"

class Client
{
    public:
        Client(SemWrapper& semHost, SemWrapper& semClient, bool verbose);
	virtual ~Client();
        void run();
    private:
        static const size_t maxBufSize = 11;
        bool needStop = false;
	bool verbose = false;
        SemWrapper& semHost;
	SemWrapper& semClient;
        Conn* connection;
};

#endif // CLIENT_H
