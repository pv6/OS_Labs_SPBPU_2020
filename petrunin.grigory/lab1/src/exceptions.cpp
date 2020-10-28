#include "../include/exceptions.h"

char const* ForkException::what() const noexcept {
    return "ERROR: fork() call failed";
}

char const* SetSessionIDException::what() const noexcept {
    return "ERROR: setsid() call failed";
}

char const* NullPtrException::what() const noexcept {
    return "ERROR: unexpected nullptr passed as parameter";
}

char const* OpenFileException::what() const noexcept {
    return "ERROR: an error has occured during opening file";
}

char const* DuplicateFidException::what() const noexcept {
    return "ERROR: an error has occured during dup() call";
}

char const* ChangeDirException::what() const noexcept {
    return "ERROR: an error has occured during chdir() call";
}

char const* SignalException::what() const noexcept {
    return "ERROR: an error has occured during signal() call";
}

char const* InvalidPathException::what() const noexcept {
    return "ERROR: invalid path specified or not enough permissions";
}

char const* InvalidConfigException::what() const noexcept {
    return "ERROR: invalid config file";
}

char const* InvalidDirException::what() const noexcept {
    return "ERROR: specified directory doesn't exist";
}

char const* InvalidIntervalException::what() const noexcept {
    return "ERROR: specified interval is invalid (> 0 required)";
}

char const* SystemException::what() const noexcept {
    return "ERROR: an error has occured during system() call";
}
