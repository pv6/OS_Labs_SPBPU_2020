#ifndef MY_EXCEPTION_H_INCLUDED
#define MY_EXCEPTION_H_INCLUDED
#include <exception>
#include <string>

class SysException : public std::exception {
public:
    SysException(const std::string& msg, int errnum);
    const char* what() const noexcept override;

private:
    std::string _msg;
};


#endif // MY_EXCEPTION_H_INCLUDED
