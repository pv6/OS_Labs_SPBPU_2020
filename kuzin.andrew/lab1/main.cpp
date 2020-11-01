#include "Manager.h"

int main()
{
    Manager& mn = Manager::getInstance();
    mn.run();

    return 0;
}
