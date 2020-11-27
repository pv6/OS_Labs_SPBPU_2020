//
// Created by yudzhinnsk on 11/25/2020.
//


#include <syslog.h>
#include <unistd.h>
#include "Wolf.h"
#include <iostream>


int main(int argc, char**argv) {
    int n;
    std::cout << "Hello! It's A WOOOLF vs GOATSS GAAAAAAME!!!!!" << std::endl;
    std::cout << "Please enter count of Goats -> ";
    std::cin >> n;
    Wolf *server = Wolf::GetInstance();
    syslog(LOG_INFO, "Host started with pid %d.", getpid());
    server->SetClientsCount(n);
    syslog(LOG_INFO, "Number of clients is set to %d", n);
    server->Start();
    syslog(LOG_INFO, "host: stopped.");
    closelog();

    return 0;
}
