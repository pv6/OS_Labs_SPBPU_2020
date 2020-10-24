#ifndef VIA8_EXCEPTIONS_H
#define VIA8_EXCEPTIONS_H

#include <exception> // exception
#include <string>    // string

class ForkException final : public std::exception {
public:
    virtual char const* what() const noexcept override;
};

class SetSessionIDException final : public std::exception {
public:
    virtual char const* what() const noexcept override;
};

class NullPtrException final : public std::exception {
public:
    virtual char const* what() const noexcept override;
};

class OpenFileException final : public std::exception {
public:
    virtual char const* what() const noexcept override;
};

class DuplicateFidException final : public std::exception {
public:
    virtual char const* what() const noexcept override;
};

class ChangeDirException final : public std::exception {
public:
    virtual char const* what() const noexcept override;
};

class SignalException final : public std::exception {
public:
    virtual char const* what() const noexcept override;
};

class InvalidPathException final : public std::exception {
public:
    virtual char const* what() const noexcept override;
};

class InvalidConfigException final : public std::exception {
public:
    virtual char const* what() const noexcept override;
};

class InvalidDirException final : public std::exception {
public:
    virtual char const* what() const noexcept override;
};

class InvalidIntervalException final : public std::exception {
public:
    virtual char const* what() const noexcept override;
};

class SystemException final : public std::exception {
public:
    virtual char const* what() const noexcept override;
};

#endif /* VIA8_EXCEPTIONS_H */
