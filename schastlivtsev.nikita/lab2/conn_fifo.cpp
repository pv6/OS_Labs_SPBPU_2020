#include "conn_fifo.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <cstring>

const std::string conn_fifo::fifoPath = "/tmp/lab2.txt";

Conn::Conn() {
}

Conn::~Conn() {
}

Conn* Conn::createConnection(bool isHost) {
	// conn_fifo implementation
	return new conn_fifo(isHost);
}

conn_fifo::conn_fifo(bool isHost) : isHost(isHost) {
    // ctor
    if (isHost) {
	remove(fifoPath.c_str());
    	int rc = mkfifo(fifoPath.c_str(), 0777);
	checkError(rc, "mkfifo");
	syslog(LOG_INFO, "Host created MKFIFO file");
    }
    fileDescr = open(fifoPath.c_str(), O_RDWR);
    checkError(fileDescr, "open");
    syslog(LOG_INFO, "conn_fifo: connection opened");
}

conn_fifo::~conn_fifo() {
    // dtor
    close(fileDescr);
    syslog(LOG_INFO, "conn_fifo: connection closed");
    if (isHost) {
	remove(fifoPath.c_str());
	syslog(LOG_INFO, "conn_fifo: MKFIFO file removed");
    }
}


void conn_fifo::Write(const char* buf, const size_t len) {
   if (buf == nullptr)
	   throw std::runtime_error("Nullptr passed as buf into Conn::Write. Expected pointer to allocated buffer");
   if (len == 0)
	   throw std::runtime_error("Wrong len passed into Conn::Write. Expected positive count");
   if (len > msgMaxSize)
	   throw std::runtime_error("Too big len passed into Conn::Write");
   int rc = write(fileDescr, buf, len);
   checkError(rc, "Write");
   syslog(LOG_INFO, "conn_fifo: message sent");
}


void conn_fifo::Read(char* buf, const size_t len) {
    if (buf == nullptr)
	    throw std::runtime_error("Nullptr passed as buf into Conn::Read. Expected pointer to allocated buffer");
    if (len == 0)
	    throw std::runtime_error("Wrong len passed into Conn::Read. Expected positive count");
    if (len > msgMaxSize)
	    throw std::runtime_error("Too big len passed into Conn::Read");
    int rc = read(fileDescr, buf, len);
    checkError(rc, "Read");
    syslog(LOG_INFO, "conn_fifo: message received");
}

void conn_fifo::checkError(const int rc, const std::string& caller) const {
    if (rc != -1)
	return;  // no error
    std::string errMsg = "conn_fifo::" + caller + " returned error: " + std::strerror(errno);
    throw std::runtime_error(errMsg);
}

