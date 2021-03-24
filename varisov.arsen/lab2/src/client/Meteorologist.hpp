#ifndef LAB2_METEOROLOGIST_HPP
#define LAB2_METEOROLOGIST_HPP

#include "../util/Connection.hpp"
#include "../util/Semaphore.hpp"

class Meteorologist
{
public:
    bool run(std::shared_ptr<Connection> connection, Semaphore& hostSemaphore, Semaphore& clientSemaphore);
    Meteorologist();
private:
    int randomFactor;
};

#endif //LAB2_METEOROLOGIST_HPP
