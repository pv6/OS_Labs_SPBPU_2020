#include "../include/custom_exception.h"

#include <string> // to_string

CustomException::CustomException(char const* msg, char const* file, int line) noexcept :
    m_msg(msg != nullptr ? msg : ""), m_file(file != nullptr ? file : ""), m_line(line) {}

char const* CustomException::what() const noexcept {
    return (
        "\n"
        "ERROR: " + m_msg +
        "\n"
        "FILE : " + m_file +
        "\n"
        "LINE : " + std::to_string(m_line) + 
        "\n"
        ).c_str();
}
