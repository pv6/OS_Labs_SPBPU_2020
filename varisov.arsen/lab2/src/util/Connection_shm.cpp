#include "Connection.hpp"

#include <cstring>
#include <fcntl.h>
#include <syslog.h>
#include <system_error>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "DataPacket.hpp"

namespace
{
constexpr size_t padding    = 2;
constexpr size_t maxShmSize = sizeof(DataPacket) + padding;
constexpr size_t mmapOffset = 0;

class ShmConnection : public Connection
{
public:
    ShmConnection(int id_);
    virtual ~ShmConnection();
    virtual bool readData(void* buffer, size_t size);
    virtual bool writeData(void* buffer, size_t size);
private:
    std::string  shmName;
    int          shmDescriptor;
    void*        mappedArea;
};
}

ShmConnection::ShmConnection(int id_) : Connection(id_)
{
    shmName           = "/Lab2Connection_shm_" + std::to_string(id);
    shmDescriptor     = shm_open(shmName.c_str(), O_CREAT | O_RDWR, DEFFILEMODE);

    if (shmDescriptor == -1)
    {
        syslog(LOG_ERR, "Could not establish shared memory, connection ID : %d. Reason: %s", id, strerror(errno));
        throw std::system_error();
    }

    ftruncate(shmDescriptor, maxShmSize);
    mappedArea = mmap(nullptr, maxShmSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmDescriptor, mmapOffset);
    if (mappedArea == MAP_FAILED)
    {
        syslog(LOG_ERR, "Could not create a mapping in the shared memory, connection ID: %d."
                                                                                " Reason: %s", id, strerror(errno));
        close(shmDescriptor);
        throw std::system_error();
    }

    syslog(LOG_INFO, "Established shm connection. Connection ID: %d", id);
}

bool ShmConnection::readData(void* buffer, size_t size)
{
    return std::memcpy(buffer, mappedArea, size) != nullptr;
}

bool ShmConnection::writeData(void* buffer, size_t size)
{
    return std::memcpy(mappedArea, buffer, size);
}

ShmConnection::~ShmConnection()
{
    munmap(mappedArea, maxShmSize);
    close(shmDescriptor);
    shm_unlink(shmName.c_str());
    syslog(LOG_INFO, "Closed shm connection. Connection ID: %d", id);
}

std::shared_ptr<Connection> Connection::establish(int id)
{
    return std::make_shared<ShmConnection>(id);
}
