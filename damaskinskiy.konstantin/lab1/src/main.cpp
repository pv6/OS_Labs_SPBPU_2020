#include <iostream>
#include "config.h"

int main( int argc, char **argv )
{
    if (argc != 2)
    {
        std::cerr << "Need argument <path_to_config>!\n";
        return 1;
    }

    Config config(argv[1]);
	std::cout << "Hello world\n";
	return 0;
}

