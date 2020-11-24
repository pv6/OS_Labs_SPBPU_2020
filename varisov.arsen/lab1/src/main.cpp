#include <iostream>

#include "DirectoryCleaner.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "User should provide path to a dir as argument\n";
        return 1;
    }

    try
    {
        DirectoryCleaner& cleaner = DirectoryCleaner::instance(argv[1]);
        cleaner.run();
    }
    catch(const std::exception& e)
    {
        return 1;
    }
    
    return 0;
}
