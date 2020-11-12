#include <iostream>
#include <exception>
#include <string>
#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "HostWorker.h"
#include "Client.h"
#include "SemWrapper.h"

/*
 * Executable should be called with 2 or 3 arguments
 * Arg 1: path to the text file with dates in format dd.mm.yyyy
 * Arg 2: path to the resulting text file where predictions will be stored
 * Arg 3 (optional): verbose mode, 1 - all IPC-messages will be printed to stdout, another value (or missed value) - quiet mode, don't use stdout.
 * Errors and statuses will be reported via syslog (regardless verbose mode)
 * */

int main(int argc, char** argv)
{
    static const int success = 0;
    static const int error = 1;
    
    openlog("lab2", LOG_PID | LOG_NDELAY, LOG_USER);
    syslog(LOG_INFO, "Program launched");
    try {
        if (argc < 3 || argc > 4)
            throw std::runtime_error("Input and output files expected to be cmd line args");
	bool verbose = (argc == 4 && argv[3][0] == '1');
	if (verbose)
	    std::cout << "Lab2 launched in verbose mode\n";
	SemWrapper semHost("/lab2host");
	semHost.post();
	SemWrapper semClient("/lab2client");
        pid_t pid = fork(); // create client

        if (pid < 0)
            throw std::runtime_error("Fork for the Client has not been created (PID < 0)");
        else if (pid > 0) {
            // it's a parent process => host
            syslog(LOG_INFO, "Host: fork created successfully");
            Host host = Host(argv[1], argv[2], semHost, semClient, verbose);
            host.run();
        }
        else {
            // it's a child process => client
            syslog(LOG_INFO, "Client: fork created successfully");
            Client client = Client(semHost, semClient, verbose);
            client.run();
        }
    } catch(std::runtime_error& err) {
        std::string errMsg("Exception caught: ");
        errMsg += err.what();
        syslog(LOG_ERR, "%s", errMsg.c_str());
        closelog();
        return error;
    } catch(...) {
        syslog(LOG_ERR, "Unknown exception caught");
        closelog();
        return error;
    }

    syslog(LOG_INFO, "Process FINISHING");
    closelog();
    return success;
}
