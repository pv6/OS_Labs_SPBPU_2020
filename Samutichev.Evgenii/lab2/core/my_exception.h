#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED
#include <exception>
#include <string>

class MyException : public std::exception {
public:
    enum class Type {
        WARNING, CRITICAL
    };

    MyException(const std::string& msg, Type type);
    const char* what() const noexcept override;
    Type getType();

private:
    std::string _msg;
    Type _type;
};


#endif // ERROR_H_INCLUDED
