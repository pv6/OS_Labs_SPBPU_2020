#include "../client/Meteorologist.hpp"
#include "MeteorologyHost.hpp"

#include <unistd.h>
#include <syslog.h>
#include <cstdio>

int main()
{
    int connectionId = 1;
    std::vector<DataPacket> dataPackets = { //Day, month, year, isLastPacket
                                            DataPacket(17, 3, 1999, false)
                                          , DataPacket(15, 2, 2000, false)
                                          , DataPacket(29, 11, 1985, false)
                                          , DataPacket(29, 11, 1985, false)
                                          , DataPacket(10, 1, 1934, false)
                                          , DataPacket(15, 12, 1975, false)
                                          , DataPacket(5, 12, 2015, true)
                                          };
    openlog("Lab2", LOG_PID, LOG_DAEMON);
    try
    {
        Semaphore semHost, semClient;
        std::shared_ptr<Connection> connection = Connection::establish(connectionId);
        pid_t pid = fork();
        if (pid < 0)
        {
            syslog(LOG_ERR, "A problem occured during fork() call");
            return -1;
        }

        if (pid == 0)
        {
            Meteorologist client;
            return client.run(connection, semHost, semClient);
        }
        else
        {
            MeteorologyHost host(dataPackets);
            return host.run(connection, semHost, semClient);
        }
    }
    catch(const std::exception& e)
    {
        syslog(LOG_ERR, "A system error occurred during initialization. See the logs for more info");
        return -2;
    } 
}
