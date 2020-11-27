#include "../include/custom_exception.h"

#include <string> // to_string

CustomException::CustomException(char const* what, char const* file, int line, int errn) noexcept :
    m_what(what != nullptr ? ("ERROR: " + std::string(what)) : ""),
    m_errn(errn != 0 ? ("ERRNO: " + std::to_string(errn)) : ""),
    m_file(file != nullptr ? ("FILE : " + std::string(file)) : ""),
    m_line(file != nullptr ? ("LINE : " + std::to_string(line)) : "") {}

char const* CustomException::what() const noexcept {
    return m_what.c_str();
}

char const* CustomException::errn() const noexcept {
    return m_errn.c_str();
}

char const* CustomException::file() const noexcept {
    return m_file.c_str();
}

char const* CustomException::line() const noexcept {
    return m_line.c_str();
}
