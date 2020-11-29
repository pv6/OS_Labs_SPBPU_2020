#include "connect.h"
#include "message.h"
#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>
#include <syslog.h>
#define MQ_NAME "/mq"

connection::connection() : is_open(false)
{
    desc = new int (-1);
}

connection::~connection()
{
    delete desc;
}

bool connection::open_connect(size_t id, bool create)
{
    if (desc == nullptr)
        return false;
    if (is_open)
        return true;
    need_to_rm = create;

    if (create)
    {
        syslog(LOG_NOTICE, "mq: creating conn with id %lu.", id);
        mq_unlink((MQ_NAME + std::to_string(id)).c_str());
        struct mq_attr attr = {0, 10, MESSAGE_SIZE, 0};
        syslog(LOG_NOTICE, "mq: desc is %d.", *desc);
        *desc = mq_open((MQ_NAME + std::to_string(id)).c_str(), O_CREAT | O_RDWR, 0666, &attr);
        syslog(LOG_NOTICE, "mq: desc is %d.", *desc);
    }
    else
    {
        syslog(LOG_NOTICE, "mq: getting conn with id %lu.", id);
        *desc = mq_open((MQ_NAME + std::to_string(id)).c_str(), O_RDWR);
    }

    is_open = (*desc != -1);
    if (is_open)
        syslog(LOG_ERR, "mq: opened conn with id %lu.", id);
    else
    {
        syslog(LOG_ERR, "mq: couldn't open conn with id %lu.", id);
        if (need_to_rm)
            mq_unlink((MQ_NAME + std::to_string(id)).c_str());
    }

    return is_open;
}

bool connection::recv_connect(void *buf, size_t count)
{
    if (!is_open)
    {
        syslog(LOG_ERR, "mq: couldn't read data.");
        return false;
    }
    if (mq_receive(*desc, (char *)buf, count, nullptr) == -1)
    {
        syslog(LOG_ERR, "mq: mq_receive failed.");
        return false;
    }
    syslog(LOG_NOTICE, "mq: read data.");
    return true;
}

bool connection::send_connect(void* buf, size_t count)
{
    if (!is_open || count > MESSAGE_SIZE)
    {
        syslog(LOG_ERR, "mq: couldn't write data.");
        return false;
    }
    if (mq_send(*desc, (char *)buf, count, 0) == -1)
    {
        syslog(LOG_ERR, "mq: mq_send failed.");
        return false;
    }
    syslog(LOG_NOTICE, "mq: wrote data.");
    return true;
}

bool connection::close_connect()
{
    if (is_open && !mq_close(*desc) && (!need_to_rm || !mq_unlink(MQ_NAME)))
    {
        syslog(LOG_NOTICE, "mq: closed.");
        is_open = false;
    }
    return !is_open;
}