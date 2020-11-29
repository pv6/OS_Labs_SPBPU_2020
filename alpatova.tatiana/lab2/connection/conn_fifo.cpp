#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>

#include "connect.h"

#define FIFO_PATH "fifo"



connection::connection() : need_to_rm(true), is_open(false)
{
    desc = new int(-1);
}

connection::~connection()
{
    delete desc;
}

bool connection:: open_connect(size_t id, bool create)
{
    if (desc == nullptr)
        return false;

    if (is_open)
        return true;

    need_to_rm = create;

    if (create)
    {
        syslog(LOG_NOTICE, "fifo: creating conn with id %lu.", id);
        unlink((FIFO_PATH + std::to_string(id)).c_str());
        if (mkfifo((FIFO_PATH + std::to_string(id)).c_str(), 0666) == -1)
        {
            syslog(LOG_ERR, "fifo: mkfifo failed.");
            return (is_open = false);
        }
    }
    else
        syslog(LOG_NOTICE, "fifo: getting conn with id %lu.", id);

    *desc = open((FIFO_PATH + std::to_string(id)).c_str(), O_RDWR);

    is_open = (*desc != -1);
    if (is_open)
        syslog(LOG_ERR, "fifo: opened conn with id %lu.", id);
    else
    {
        syslog(LOG_ERR, "fifo: couldn't open conn with id %lu.", id);
        if (need_to_rm)
            unlink((FIFO_PATH + std::to_string(id)).c_str());
    }

    return is_open;
}

bool connection::close_connect()
{
    if (is_open && !close(*desc) && (!need_to_rm || !unlink(FIFO_PATH)))
    {
        syslog(LOG_NOTICE, "fifo: closed.");
        is_open = false;
    }
    return !is_open;
}

bool connection::recv_connect(void *buf, size_t count)
{
    if (!is_open)
    {
        syslog(LOG_ERR, "fifo: couldn't read data.");
        return false;
    }
    if (read(*desc, buf, count) == -1)
    {
        syslog(LOG_ERR, "fifo: read failed.");
        return false;
    }
    syslog(LOG_NOTICE, "fifo: read data.");
    return true;
}

bool connection::send_connect(void *buf, size_t count)
{
    if (!is_open)
    {
        syslog(LOG_ERR, "fifo: couldn't write data.");
        return false;
    }
    if (write(*desc, buf, count) == -1)
    {
        syslog(LOG_ERR, "fifo: write failed.");
        return false;
    }
    syslog(LOG_NOTICE, "fifo: wrote data.");
    return true;
}
