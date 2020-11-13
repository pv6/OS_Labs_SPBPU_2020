#ifndef VIA8_CUSTOM_EXCEPTION_H
#define VIA8_CUSTOM_EXCEPTION_H

#include <exception> // exception
#include <string>    // string

class CustomException final : public std::exception {
private:
    std::string m_msg;
    std::string m_file;
    int m_line;
    
public:
    explicit CustomException(char const* msg, char const* file, int line) noexcept;
    virtual char const* what() const noexcept override;
};

#endif /* VIA8_CUSTOM_EXCEPTION_H */