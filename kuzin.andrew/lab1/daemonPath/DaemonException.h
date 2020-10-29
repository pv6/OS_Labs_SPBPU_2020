
#ifndef DAEMON_EXCEPTION_H
#define DAEMON_EXCEPTION_H

#include <string>

#include "types.h"

class DaemonException : std::exception {
private:
    std::string msg_;
    bool err_;
public:
    DaemonException(std::string &msg, bool err) : msg_(msg), err_(err) {};
    std::string getMsg() const noexcept { return msg_; }
    bool isFatalError() const noexcept { return err_; }
};

#endif //DAEMON_EXCEPTION_H
