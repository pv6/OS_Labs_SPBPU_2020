#include "daemon.h"

Daemon* Daemon::instance = 0;

Daemon* Daemon::get_instance(const char* cfg_path)
{
    if (!instance) {
        char* cfg_abs_path = realpath(cfg_path, nullptr);
        instance = new Daemon(cfg_abs_path);
        free(cfg_abs_path);
    }
    return instance;
}

Daemon::Daemon(const char* cfg_path) :
        pid_file("/var/run/daemon.pid"), cfg_path(cfg_path), interval(10)
{
    kill_prev_instance();
    read_config();
}

void Daemon::execute() {
    syslog(LOG_NOTICE, "Start execution");

    while (1) {
        syslog(LOG_NOTICE, "Check events");

        time_t t = time(0);
        for (auto it = events.begin(); it != events.end(); it++) {
            if ((*it).check_time(t, interval)) {
                print_event_text((*it).remind(t));
            }
        }

        sleep(interval);
    }
}

void Daemon::print_event_text(const std::string& text) {
    system(std::string(
            ("gnome-terminal --working-directory='/home' -- sh -c \"echo ") + text +
            std::string("; read line\"")).c_str());
}

void Daemon::read_config()
{
    events.clear();
    std::ifstream cfg = std::ifstream(cfg_path);

    std::string line;
    while (std::getline(cfg, line)) {
        events.push_back(Event::parse_event(line));
    }

    cfg.close();
}

void Daemon::kill_prev_instance()
{
    std::ifstream in(pid_file);
    if (in.is_open() && !in.eof()) {
        pid_t p;
        in >> p;
        if (p > 0)
            kill(p, SIGTERM);
    }
    in.close();
    std::ofstream out(pid_file);
    if (!out) {
        syslog(LOG_ERR, "Creation pid file failed");
        exit(EXIT_FAILURE);
    }
    else {
        out << getpid();
        out.close();
    }
}

void Daemon::signal_handler(int sgnl)
{
    switch(sgnl)
    {
        case SIGHUP:
            if (instance) instance->read_config();
            syslog(LOG_NOTICE, "Hangup Catched");
            break;
        case SIGTERM:
            syslog(LOG_NOTICE, "Terminate Catched");
            if (instance) unlink(instance->pid_file.c_str());
            exit(EXIT_SUCCESS);
    }
}