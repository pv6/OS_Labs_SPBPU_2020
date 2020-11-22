#ifndef CONN_FIFO_H
#define CONN_FIFO_H

#include "Conn.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string>

class conn_fifo : public Conn
{
    public:
	conn_fifo(bool isHost);
	virtual ~conn_fifo();

	void Read(char* buf, const size_t len) override;
	void Write(const char* buf, const size_t len) override;

    private:
	// fields
	static const size_t msgMaxSize = 11;
	bool isHost;
	static const std::string fifoPath; 
	int fileDescr;

	// functions
	void checkError(const int rc, const std::string& caller) const;
};

#endif // CONN_FIFO_H
