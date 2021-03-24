#include "Connection.hpp"

#include <cerrno>
#include <cstring>
#include <system_error>
#include <syslog.h>
#include <unistd.h>

namespace
{
class PipeConnection : public Connection
{
public:
    PipeConnection(int id_);
    virtual ~PipeConnection();
    virtual bool readData(void* buffer, size_t size);
    virtual bool writeData(void* buffer, size_t size);
private:
    int pipeFileDescriptors[2];
};
}

PipeConnection::PipeConnection(int id_) : Connection(id_)
{
    if (pipe(pipeFileDescriptors) == -1)
    {
        syslog(LOG_ERR, "Could not establish a pipe connection. Reason: %s", strerror(errno));
        throw std::system_error();
    }
    syslog(LOG_INFO, "Established a pipe connection. Connection ID: %d", id);
}

bool PipeConnection::readData(void* buffer, size_t size)
{
    if (!read(pipeFileDescriptors[0], buffer, size))
    {
        syslog(LOG_ERR, "Could not read data from pipe. Reason: %s", strerror(errno));
        return false;
    }
    return true;
}

bool PipeConnection::writeData(void* buffer, size_t size)
{
    if (!write(pipeFileDescriptors[1], buffer, size))
    {
        syslog(LOG_ERR, "Could not write data to pipe. Reason: %s", strerror(errno));
        return false;
    }
    return true;
}

PipeConnection::~PipeConnection()
{
    if (!close(pipeFileDescriptors[0]) || !close(pipeFileDescriptors[1]))
        syslog(LOG_ERR, "Could not close the pipe. Connection ID: %d", id);
    else
        syslog(LOG_INFO, "Closed pipe connection. Connection ID: %d", id);
}

std::shared_ptr<Connection> Connection::establish(int id)
{
    return std::make_shared<PipeConnection>(id);
}