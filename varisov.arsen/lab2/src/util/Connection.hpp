#ifndef LAB2_CONNECTION_HPP
#define LAB2_CONNECTION_HPP

#include <cstddef>
#include <memory>

class Connection
{
public:
    static std::shared_ptr<Connection> establish(int id);

    Connection(int id_) : id(id_) {};
    virtual ~Connection() {};
    virtual bool readData(void* buffer, size_t size) = 0;
    virtual bool writeData(void* buffer, size_t size) = 0;

    Connection& operator=(Connection&) = delete;
    Connection(Connection&)            = delete;
protected:
    int id;
};

#endif //LAB2_CONNECTION_HPP
