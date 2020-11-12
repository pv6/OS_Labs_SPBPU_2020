#ifndef CONN_SOCK_H
#define CONN_SOCK_H

#include "Conn.h"

#include <sys/types.h>
#include <cstdio>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>

class conn_sock : public Conn {
    public:
	conn_sock(bool isHost);
	virtual ~conn_sock();

	void Read(char* buf, const size_t len) override;
	void Write(const char* buf, const size_t len) override;

    private:
	//fields
	static const size_t msgMaxSize = 11;
	static const std::string sockName;
	bool isHost;
	int connSocket;
	int dataSocket;

	// functions
	void checkErrors(const int rc, const std::string& caller) const;
	void initHost();
	void initClient();
	struct sockaddr_un createAddrStruct();
};

#endif // CONN_SOCK_H
