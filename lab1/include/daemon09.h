#ifndef VIA8_DAEMON_09_H
#define VIA8_DAEMON_09_H

#include "daemon.h"
#include <string> // string, stoi

class Daemon09 final : public Daemon {
private:
    std::string m_dir_from;
    std::string m_dir_to;
    int m_interval_s;

    void configure();

public:
    explicit Daemon09(std::string const& config_path, std::string const& pid_path);
    void reconfigure() override;
    void work() override;

private:
    Daemon09(Daemon09 const&) = delete;
    Daemon09& operator=(Daemon09 const&) = delete;
};

#endif /* VIA8_DAEMON_09_H */
