#include "Goat.h"

int main(int argc, char* argv[])
{
    Goat& goat = Goat::getInstance();

    try
    {
        goat.run(argc, argv);
    } catch (const std::exception &e)
    {
        std::cout << "ERROR:client can't start" << std::endl;
        return 1;
    }
    return 0;
}

