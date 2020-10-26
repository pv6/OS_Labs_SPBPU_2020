//
// Created by Daria on 10/26/2020.
//

#ifndef DAEMON_LAB1_USER_EXCEPTION_H
#define DAEMON_LAB1_USER_EXCEPTION_H

#include <cstdlib>
#include <exception>
#include <string>

class user_exception : std::exception {
private:
    std::string message_;
    bool error_exit_;
public:
    user_exception(std::string &msg, bool exit_e) : message_(msg), error_exit_(exit_e) {};
    std::string get_message() noexcept { return message_; }
    bool is_exit_error() noexcept { return error_exit_; }
};

#endif //DAEMON_LAB1_USER_EXCEPTION_H
