#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H

#include <exception>
#include <string>

class MyException: public std::exception
{
public:
    MyException(const std::string& msg) : std::exception(), msg(msg) {}
    virtual const char* what() const throw()
    {
        return msg.c_str();
    }

private:
    std::string msg;
};
#endif //MYEXCEPTION_H
