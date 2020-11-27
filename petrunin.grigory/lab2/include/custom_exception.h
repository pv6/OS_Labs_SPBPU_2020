#ifndef VIA8_CUSTOM_EXCEPTION_H
#define VIA8_CUSTOM_EXCEPTION_H

#include <exception> // exception
#include <string>    // string

class CustomException final : public std::exception {
private:
    std::string m_what;
    std::string m_errn;
    std::string m_file;
    std::string m_line;
    
public:
    explicit CustomException(char const* msg, char const* file = nullptr, int line = 0, int errn = 0) noexcept;
    virtual char const* what() const noexcept override;

    char const* errn() const noexcept;
    char const* file() const noexcept;
    char const* line() const noexcept;
};

#endif /* VIA8_CUSTOM_EXCEPTION_H */
