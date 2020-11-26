#include "my_client.h"
#include <syslog.h>
#include <string>

int main(int argc, char* argv[])
{
    openlog("lab2", LOG_NOWAIT | LOG_PID | LOG_PERROR, LOG_LOCAL1);
    syslog(LOG_NOTICE, "client: started.");
    if (argc != 2)
    {
        syslog(LOG_ERR, "client: couldn't find pid of host.");
        closelog();
        return EXIT_FAILURE;
    }
    int host_pid = std::stoi(argv[1]);

    my_client &client = my_client::get_instance(host_pid);
    if (client.open_connection())
        client.run();
    else
    {
        syslog(LOG_NOTICE, "client: stopped.");
        closelog();
    }
    return EXIT_SUCCESS;
}
