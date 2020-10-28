
#include "parser.h"
#include <iostream>
#include <unistd.h>
#include "daemon.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Usage lab1 config" << endl;
        return -1;
    }
    //(gdb) set follow-fork-mode child
    //(gdb) set detach-on-fork off

    if (!daemon::startDaemonization(argv[1]))
        return -1;

    return 0;
}