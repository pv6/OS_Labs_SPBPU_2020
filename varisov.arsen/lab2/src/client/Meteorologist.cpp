#include "Meteorologist.hpp"

#include <cstdlib>
#include <ctime>
#include <syslog.h>

#include "../util/DataPacket.hpp"

namespace
{
const int secondsToWaitForSemaphore = 30;
}

Meteorologist::Meteorologist()
{
    std::srand(std::time(nullptr));
    randomFactor = std::rand();
}

bool Meteorologist::run(std::shared_ptr<Connection>connection, Semaphore& hostSemaphore, Semaphore& clientSemaphore)
{
    bool shouldBeRunning = true;
    DataPacket data;
    while (shouldBeRunning)
    {
        syslog(LOG_INFO, "Meteorologist releasing semaphore for host");
        hostSemaphore.post();
        syslog(LOG_INFO, "Meteorologist attemtping to acquire semaphore for reading");
        if (clientSemaphore.timedWait(secondsToWaitForSemaphore))
        {
            if (!connection->readData(&data, sizeof(data)))
            {
                syslog(LOG_ERR, "Meteorologist failed to read the data packet");
                return false;
            }
            syslog(LOG_INFO, "Meteorologist successfully read the data packet");
            shouldBeRunning = !data.isLastPacket;

            data.temperature = (randomFactor * (data.year + data.month + data.day) % 80 - 30);
            syslog(LOG_INFO, "Client setting the following temperature:%d", data.temperature);
            if (!connection->writeData(&data, sizeof(data)))
            {
                syslog(LOG_ERR, "Meteorologist failed to write the prediction");
                return false;
            }
            syslog(LOG_INFO, "Meteorologist successfully wrote the prediction");
            hostSemaphore.post();
        }
        else
        {
            syslog(LOG_ERR, "Meteorologist could not acquire semaphore in prescribed time");
            return false;
        }
    }
    syslog(LOG_INFO, "Meteorologist successfully finished execution");
    return true;
}

