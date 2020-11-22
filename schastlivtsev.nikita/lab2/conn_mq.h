#ifndef CONN_MQ_H
#define CONN_MQ_H

#include "Conn.h"

#include <sys/types.h>
#include <mqueue.h>
#include <cstdio>
#include <string>

class conn_mq : public Conn
{
    public:
        conn_mq(bool isHost);
        virtual ~conn_mq();

        void Read(char* buf, const size_t len) override;
        void Write(const char* buf, const size_t len) override;
    protected:

    private:
        // fields
        mqd_t mq;
        static const size_t msgMaxSize = 11;
	static const std::string queueName;
	bool isHost;

        // functions
        inline void OpenConn();
	void checkError(const int rc, const std::string& caller) const;
};

#endif // CONN_MQ_H
