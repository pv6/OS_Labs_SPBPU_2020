#include "MeteorologyHost.hpp"

#include <cstdio>
#include <syslog.h>

namespace
{
const int secondsToWaitForSemaphore = 30;
}

bool MeteorologyHost::run(std::shared_ptr<Connection> connection, Semaphore& hostSemaphore, Semaphore& clientSemaphore)
{
    syslog(LOG_INFO, "Meteorology host started running");
    for (DataPacket& data : dataList)
    {
        syslog(LOG_INFO, "Meteorology host attempting to acquire semaphore");
        if(hostSemaphore.timedWait(secondsToWaitForSemaphore))
        {
            syslog(LOG_INFO, "Meteorology host sending data over to client");
            if (!connection->writeData(&data, sizeof(data)))
            {
                syslog(LOG_ERR, "Meteorology host could not write data packet");
                return false;
            }
            clientSemaphore.post();

            syslog(LOG_INFO, "Meteorology host acquiring semaphore to read the prediction");
            if (hostSemaphore.timedWait(secondsToWaitForSemaphore))
            {
                syslog(LOG_INFO, "Meteorology host reading the prediction");
                if (!connection->readData(&data, sizeof(data)))
                {
                    syslog(LOG_ERR, "Meteorology host could not read data packet");
                    return false;
                }
                if (data.temperature == uninitializedTemp)
                {
                    syslog(LOG_ERR, "Meteorology host received uninitialized temperature value");
                    return false;
                }
                printf("Predicted temperature for %02d.%02d.%02d: %d\n", 
                       data.day, data.month, data.year, data.temperature);
            }
        }
        else
        {
            syslog(LOG_ERR, "Meteorology host could not acquire semaphore in prescribed time");
            return false;
        }
    }
    
    syslog(LOG_INFO, "Meteorology host successfully processed all temperature predictions. Finishing execution");
    return true;
}
