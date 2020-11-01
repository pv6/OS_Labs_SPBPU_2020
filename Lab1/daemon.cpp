#include "daemon.h"

Daemon* Daemon::instance = 0;

Daemon* Daemon::init(int argc,char **argv) {
    openlog("Daemon: ", LOG_NOWAIT | LOG_PID, LOG_USER);
    check_args(argc);
    do_fork();

    Daemon* daemon = Daemon::get_instance(argv[1]);
    syslog(LOG_NOTICE, "Daemon launching is successful");
    change_dir();
    close_streams();

    signal(SIGHUP, Daemon::signal_handler);
    signal(SIGTERM, Daemon::signal_handler);

    return daemon;
}

void Daemon::check_args(int argc) {
    if (argc < 2)
    {
        syslog(LOG_ERR, "Wrong numbers of arguments. Expected: 2. Got: %d\n", argc);
        exit(EXIT_FAILURE);
    }

    syslog(LOG_NOTICE, "Args are correct");
}

void Daemon::do_fork() {
    pid_t pid = fork();

    if (pid == -1) {
        syslog(LOG_ERR, "Fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
        exit(EXIT_SUCCESS);

    umask(0);

    if (setsid() < 0)
    {
        syslog(LOG_ERR, "Generation session ID for child process failed");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        syslog(LOG_ERR, "Fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
        exit(EXIT_SUCCESS);

    syslog(LOG_NOTICE, "Fork is successful");
}

void Daemon::change_dir() {
    if ((chdir("/")) < 0)
    {
        syslog(LOG_ERR, "Changing working directory to \"/\" failed");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_NOTICE, "Changing directory is successful");
}

void Daemon::close_streams() {
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

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