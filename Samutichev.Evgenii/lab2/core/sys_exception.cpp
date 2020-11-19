#include "sys_exception.h"

SysException::SysException(const std::string& msg, int errnum) {
    _msg = msg + "; error code: " + std::to_string(errnum);
}

const char* SysException::what() const noexcept {
    return _msg.c_str();
}
