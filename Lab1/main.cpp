#include <syslog.h>
#include <csignal>

#include "daemon.h"

void check_args(int argc) {
    if (argc < 2)
    {
        syslog(LOG_ERR, "Wrong numbers of arguments. Expected: 2. Got: %d\n", argc);
        exit(EXIT_FAILURE);
    }

    syslog(LOG_NOTICE, "Args are correct");
}

void do_fork() {
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

void change_dir() {
    if ((chdir("/")) < 0)
    {
        syslog(LOG_ERR, "Changing working directory to \"/\" failed");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_NOTICE, "Changing directory is successful");
}

void close_streams() {
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main(int argc,char **argv)
{
    Daemon* daemon = nullptr;
    try {
        openlog("Daemon: ", LOG_NOWAIT | LOG_PID, LOG_USER);
        check_args(argc);
        //do_fork();

        daemon = Daemon::get_instance(argv[1]);
        syslog(LOG_NOTICE, "Daemon launching is successful");
        change_dir();
        close_streams();

        signal(SIGHUP, Daemon::signal_handler);
        signal(SIGTERM, Daemon::signal_handler);

        daemon->execute();
    }
    catch (const std::exception& err)
    {
        syslog(LOG_ERR, "%s. Work is stopping. ", err.what());
        return 1;
    }

    delete daemon;

    return 0;
}