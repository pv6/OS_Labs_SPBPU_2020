#include "my_exception.h"

MyException::MyException(const std::string& msg, Type type)
    : _msg(msg), _type(type)
{}

const char* MyException::what() const noexcept {
    return _msg.c_str();
}

MyException::Type MyException::getType() {
    return _type;
}
