#include "Connection.hpp"

#include <cstring>
#include <syslog.h>
#include <system_error>
#include <sys/mman.h>

#include "DataPacket.hpp"

namespace
{
constexpr size_t padding     = 2;
constexpr size_t mappingSize = sizeof(DataPacket) + padding;
constexpr size_t mmapOffset  = 0;
constexpr int    mmapFd      = -1; //Parent process' memory is used

class MmapConnection : public Connection
{
public:
    MmapConnection(int id_);
    virtual ~MmapConnection();
    virtual bool readData(void* buffer, size_t size);
    virtual bool writeData(void* buffer, size_t size);
private:
    void* mappedArea;
};
}

MmapConnection::MmapConnection(int id_) : Connection(id_)
{
    mappedArea = mmap(nullptr, mappingSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, mmapFd, mmapOffset);
    if (mappedArea == MAP_FAILED)
    {
        syslog(LOG_ERR, "Could not create an anonymous mapping, connection ID: %d. Reason: %s", id, strerror(errno));
        throw std::system_error();
    }
    syslog(LOG_INFO, "Established an anonymous mapping connection. Connection ID: %d", id);
}

bool MmapConnection::readData(void* buffer, size_t size)
{
    syslog(LOG_INFO, "Reading");
    return std::memcpy(buffer, mappedArea, size) != nullptr;
}

bool MmapConnection::writeData(void* buffer, size_t size)
{
    syslog(LOG_INFO, "Writing");
    return std::memcpy(mappedArea, buffer, size);
}

MmapConnection::~MmapConnection()
{
    munmap(mappedArea, mappingSize);
}

std::shared_ptr<Connection> Connection::establish(int id)
{
    return std::make_shared<MmapConnection>(id);
}