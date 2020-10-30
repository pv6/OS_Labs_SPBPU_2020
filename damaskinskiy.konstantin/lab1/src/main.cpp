#include <iostream>
#include "diskmonitor.h"

int main( int argc, char **argv )
{
    if (argc != 2)
    {
        std::cerr << "Need argument <path_to_config>!\n";
        return 1;
    }

    auto &dm = DiskMonitor::get();
    dm.init(argv[1]);
    return !dm.start();
}

