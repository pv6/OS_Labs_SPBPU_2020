#include <syslog.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <new>

#include "connect.h"

#define SERVER_PATH "/tmp/lab2_server"

connection::connection() : need_to_rm(true), is_open(false)
{
    desc = new int[2]();
}

connection::~connection()
{
    delete[] desc;
}

bool connection::open_connect(size_t id, bool create)
{
    if (desc == nullptr)
        return false;

    struct sockaddr_un saddr = {AF_UNIX, SERVER_PATH};
    if (is_open)
    {
        if (create)
        {
            if ((desc[1] = accept(desc[0], NULL, NULL)) == -1)
            {
                syslog(LOG_ERR, "sock: accept failed.");
                close(desc[0]);
                unlink(SERVER_PATH);
                return (is_open = false);
            }
            syslog(LOG_ERR, "sock: accepted.");
        }
        else
        {
            if ((connect(desc[1], (struct sockaddr *)&saddr, SUN_LEN(&saddr))) == -1)
            {
                syslog(LOG_ERR, "sock: connect failed.");
                close(desc[1]);
                return (is_open = false);
            }
            syslog(LOG_ERR, "sock: connected.");
        }
        return true;
    }

    need_to_rm = create;

    if (create)
    {
        syslog(LOG_NOTICE, "sock: creating conn with id %lu.", id);
        desc[1] = -1;
        unlink(SERVER_PATH);
        if ((desc[0] = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        {
            syslog(LOG_ERR, "sock: socket failed.");
            return (is_open = false);
        }
        if ((bind(desc[0], (struct sockaddr *)&saddr, SUN_LEN(&saddr))) == -1)
        {
            syslog(LOG_ERR, "sock: bind failed.");
            close(desc[0]);
            return (is_open = false);
        }
        if ((listen(desc[0], 1)) == -1)
        {
            syslog(LOG_ERR, "sock: listen failed.");
            close(desc[0]);
            unlink(SERVER_PATH);
            return (is_open = false);
        }
        if ((desc[1] = accept(desc[0], NULL, NULL)) == -1)
        {
            syslog(LOG_ERR, "sock: accept failed.");
            close(desc[0]);
            unlink(SERVER_PATH);
            return (is_open = false);
        }
        syslog(LOG_ERR, "sock: accepted.");
    }
    else
    {
        syslog(LOG_NOTICE, "sock: getting conn with id %lu.", id);
        desc[0] = -1;
        if ((desc[1] = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        {
            syslog(LOG_ERR, "sock: socket failed.");
            return (is_open = false);
        }
        if ((connect(desc[1], (struct sockaddr *)&saddr, SUN_LEN(&saddr))) == -1)
        {
            syslog(LOG_ERR, "sock: connect failed.");
            close(desc[1]);
            return (is_open = false);
        }
        syslog(LOG_ERR, "sock: connected.");
    }

    syslog(LOG_ERR, "sock: opened conn with id %lu.", id);
    return (is_open = true);
}

bool connection::close_connect()
{
    if ((is_open || desc[0] != -1) && (desc[1] == -1 || !close(desc[1]))
        && (desc[0] == -1 || !close(desc[0]))
        && (!need_to_rm || !unlink(SERVER_PATH)))
    {
        syslog(LOG_NOTICE, "sock: closed.");
        is_open = false;
    }
    return !is_open;
}

bool connection::recv_connect(void *buf, size_t count)
{
    if (!is_open)
    {
        syslog(LOG_ERR, "sock: couldn't read data.");
        return false;
    }
    if (recv(desc[1], buf, count, 0) == -1)
    {
        syslog(LOG_ERR, "sock: recv failed.");
        return false;
    }
    syslog(LOG_NOTICE, "sock: read data.");
    return true;
}

bool connection::send_connect(void *buf, size_t count)
{
    if (!is_open)
    {
        syslog(LOG_ERR, "sock: couldn't write data.");
        return false;
    }
    if (send(desc[1], buf, count, MSG_NOSIGNAL) == -1)
    {
        syslog(LOG_ERR, "sock: send failed.");
        return false;
    }
    syslog(LOG_NOTICE, "sock: wrote data.");
    return true;
}