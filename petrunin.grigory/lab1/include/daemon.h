#ifndef VIA8_DAEMON_H
#define VIA8_DAEMON_H

#include "exceptions.h"
#include <string> // string

class Daemon {
protected:
    std::string m_config_path;
    std::string m_pid_path;
    bool m_stop = false;

public:
    explicit Daemon(std::string const& config_path, std::string const& pid_path);
    virtual ~Daemon() {}

    void stop() { m_stop = true; }
    bool daemonize(void (*handle_signal)(int)) const;
    virtual void reconfigure() = 0;
    virtual void work() = 0;

private:
    Daemon(Daemon const&) = delete;
    Daemon& operator=(Daemon const&) = delete;
};

#endif /* VIA8_DAEMON_H */
