#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <syslog.h>
#include "NeededInfo.h"
#include "Demon.h"
#include "SubMethods.h"


int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Error: expected 2 arguments, got %d \n", argc);
        return EXIT_FAILURE;
    }

    openlog ("lab1", LOG_PID, LOG_DAEMON);
    syslog (LOG_NOTICE, "Open syslog");

    NeededInfo nf = NeededInfo(argv[1]);
    int readConfigFile = SubMethods::read_config_file(nf);
    if (readConfigFile != EXIT_SUCCESS)
        return readConfigFile;

    fflush(stdout);
    Demon dm = Demon(nf);
    dm.createDemon();
    dm.runDemon();


    syslog (LOG_NOTICE, "Close syslog");
    closelog();

    return EXIT_SUCCESS;
}