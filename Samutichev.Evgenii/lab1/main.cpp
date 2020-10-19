#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>

void sig_handler(int signum) {
    printf("Received signal %d\n", signum);
}

int main() {
    pid_t pid;

    // Fork parent process
    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    // Create SID for child process
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    signal(SIGINT, sig_handler);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    // Change the file mode mask
    umask(0);

    // Change the current working directory
    if ((chdir("/home/evgenii/Desktop/OS_Labs_SPBPU_2020/Samutichev.Evgenii/lab1/bin/Release")) < 0)
        exit(EXIT_FAILURE);

    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    FILE* f = fopen("daemon.txt", "w+");
    int i = 0;
    while(1) {
        fprintf(f, "%d", i);
        i++;
        sleep(10);
        fflush(f);
    }
    fclose(f);
    return EXIT_SUCCESS;
}
