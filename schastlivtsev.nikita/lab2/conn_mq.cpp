#include "conn_mq.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>
#include <fcntl.h>
#include <syslog.h>
#include <iostream>
#include <cstring>

const std::string conn_mq::queueName = "/labtwo";

Conn::Conn() {
}

Conn::~Conn() {
}

Conn* Conn::createConnection(bool isHost) {
    // conn_mq implementation
    return new conn_mq(isHost);
}

conn_mq::conn_mq(bool isHost) : isHost(isHost)
{
    //ctor
    OpenConn();  // have to init msg attrs
}

conn_mq::~conn_mq()
{
    //dtor
    mq_close(mq);
    syslog(LOG_INFO, "conn_mq: connection closed");
    
    if (isHost) {
    	mq_unlink(queueName.c_str());
    	syslog(LOG_INFO, "conn_mq: connection unlinked");
    }
}


void conn_mq::OpenConn() {
    // init queue attributes
    struct mq_attr attr;
    attr.mq_maxmsg = 2;
    attr.mq_msgsize = msgMaxSize;

    // open
    mq = mq_open(queueName.c_str(), O_CREAT | O_RDWR, 0777, &attr);
    checkError(mq, "OpenConn");
    syslog(LOG_INFO, "conn_mq: initialized and opened connection");
}


void conn_mq::Read(char* buf, const size_t len) {
    // validation
    if (buf == nullptr)
        throw std::runtime_error("Nullptr passed as buf into Conn::Read. Expected pointer to allocated buffer");
    if (len == 0)
        throw std::runtime_error("Wrong len passed into Conn::Read. Expected positive count");
    if (len > msgMaxSize)
        throw std::runtime_error("Too big len passed into Conn::Read");
    // reading message
    int rc = mq_receive(mq, buf, len, NULL);
    checkError(rc, "Read");
    syslog(LOG_INFO, "conn_mq: message received");
}


void conn_mq::Write(const char* buf, const size_t len) {
    // validation
    if (buf == nullptr)
        throw std::runtime_error("Nullptr passed as buf into Conn::Write. Expected pointer to allocated buffer");
    if (len == 0)
        throw std::runtime_error("Wrong len passed into Conn::Write. Expected positive count");
    if (len > msgMaxSize)
        throw std::runtime_error("Too big len passed into Conn::Write");
    // writing message
    int rc = mq_send(mq, buf, len, 0);
    checkError(rc, "Write");
    syslog(LOG_INFO, "conn_mq: message sent");
}

void conn_mq::checkError(const int rc, const std::string& caller) const {
    if (rc != -1)
	return; // no error
    std::string errMsg = "conn_mq::" + caller + " returned error: " + std::strerror(errno);
    throw std::runtime_error(errMsg);
} 
