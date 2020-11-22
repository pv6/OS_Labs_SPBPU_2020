#include "conn_sock.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <syslog.h>

const std::string conn_sock::sockName = "/tmp/lab2sock.socket";

Conn::Conn() {
}

Conn::~Conn() {
}

Conn* Conn::createConnection(bool isHost) {
    // conn_sock implementation
    return new conn_sock(isHost);
}

conn_sock::conn_sock(bool isHost) : isHost(isHost) {
    // ctor
    if (isHost)
	initHost();
    else
	initClient();
}

conn_sock::~conn_sock() {
    // dtor
    close(dataSocket);
    syslog(LOG_INFO, "Socket connection closed");
    if (isHost) {
	close(connSocket);
        unlink(sockName.c_str());
	syslog(LOG_INFO, "Socket connection unlinked");
    }
}

void conn_sock::Read(char* buf, const size_t len) {
    // validation
    if (buf == nullptr)
	throw std::runtime_error("Nullptr passed as buf into Conn::Read. Expected pointer to allocated buffer");
    if (len == 0)
	throw std::runtime_error("Wrong len passed into Conn::Read. Expected positive count");
    if (len > msgMaxSize)
	throw std::runtime_error("Too big len passed into Conn::Read");
    // reading message
    int rc = read(dataSocket, buf, len);
    checkErrors(rc, "Read");
    syslog(LOG_INFO, "conn_sock: message received");
}

void conn_sock::Write(const char* buf, const size_t len) {
    // validation
    if (buf == nullptr)
	throw std::runtime_error("Nullptr passed as buf into Conn::Write. Expected pointer to allocated buffer");
    if (len == 0)
	throw std::runtime_error("Wrong len passed into Conn::Write. Expected positive count");
    if (len > msgMaxSize)
	throw std::runtime_error("Too big len passed into Conn::Write");
    // writing message
    int rc = write(dataSocket, buf, len);
    checkErrors(rc, "Write");
    syslog(LOG_INFO, "conn_sock: message sent");
}

void conn_sock::initHost() {
    static const std::string caller = "initHost";
    // create descriptor
    connSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    checkErrors(connSocket, caller);

    // create socket address struct and bind
    struct sockaddr_un sockAddr = createAddrStruct();
    int rc = bind(connSocket, (const struct sockaddr*)&sockAddr, sizeof(struct sockaddr_un));
    checkErrors(rc, caller);
    
    // listen connections
    rc = listen(connSocket, 1);
    checkErrors(rc, caller);

    // get client descriptor
    dataSocket = accept(connSocket, NULL, NULL);
    checkErrors(dataSocket, caller);

    syslog(LOG_INFO, "Host: socket connection opened");
}

void conn_sock::initClient() {
    static const std::string caller = "initClient";
    // create descriptor
    dataSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    checkErrors(dataSocket, caller);

    // create socket address struct and connect
    struct sockaddr_un sockAddr = createAddrStruct();
    int rc = connect(dataSocket, (const struct sockaddr*)&sockAddr, sizeof(struct sockaddr_un));
    checkErrors(rc, caller);
    syslog(LOG_INFO, "Client: socket connection opened");
}

struct sockaddr_un conn_sock::createAddrStruct() {
    struct sockaddr_un sockAddr;
    
    // for portability clear the whole structure
    // (some impls have additional fields)
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sun_family = AF_UNIX;
    strncpy(sockAddr.sun_path, sockName.c_str(), sizeof(sockAddr.sun_path) - 1);    return sockAddr;
}

void conn_sock::checkErrors(const int rc, const std::string& caller) const {
    if (rc != -1)
	    return;  // no error
    std::string errMsg = "error occured in conn_sock::" + caller + ", errno = " + strerror(errno);
    throw std::runtime_error(errMsg);
}
