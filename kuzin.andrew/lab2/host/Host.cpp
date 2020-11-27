#include "Wolf.h"

int main(int argc, char* argv[])
{
    Wolf& Wolf = Wolf::getInstance();

    try
    {
        Wolf.run(argc, argv);
    } catch (const std::exception &e)
    {
        std::cout << "ERROR: host can't start" << std::endl;
        return 1;
    }
    return 0;
}

