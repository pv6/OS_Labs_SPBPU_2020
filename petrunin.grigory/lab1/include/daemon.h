#ifndef VIA8_DAEMON_H
#define VIA8_DAEMON_H

#include <string> // string
#include "worker.h"

// classic implementation of singleton
class Daemon final {
private:
    static Daemon* m_instance;
    Worker* m_worker;
    std::string m_path_pidf;

public:
    static Daemon* getDaemon(std::string const& path_pidf);
    static void handle_signal(int signal);
    bool daemonize() const;

    void worker_set(Worker* worker);
    void worker_unset();
    void worker_run();
    void worker_reconfigure();
    void worker_stop();

private:

    Daemon(std::string const& path_pidf);
    Daemon(Daemon const&) = delete;
    Daemon& operator=(Daemon const&) = delete;
};

#endif /* VIA8_DAEMON_H */
