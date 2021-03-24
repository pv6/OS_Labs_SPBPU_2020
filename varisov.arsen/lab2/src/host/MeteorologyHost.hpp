#ifndef LAB2_METEOROLOGY_HOST_HPP
#define LAB2_METEOROLOGY_HOST_HPP

#include <vector>

#include "../util/DataPacket.hpp"
#include "../util/Connection.hpp"
#include "../util/Semaphore.hpp"

class MeteorologyHost
{
public:
    MeteorologyHost(const std::vector<DataPacket>& dataList_) : dataList(dataList_) {};
    MeteorologyHost() = delete;

    bool run(std::shared_ptr<Connection> connection, Semaphore& hostSemaphore, Semaphore& clientSemaphore);
private: 
    std::vector<DataPacket> dataList;
};

#endif //LAB2_METEOROLOGY_HOST_HPP
