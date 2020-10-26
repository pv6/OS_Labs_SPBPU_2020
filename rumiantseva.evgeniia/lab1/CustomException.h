//
// Created by Evgenia on 26.10.2020.
//

#ifndef INC_1_CUSTOMEXCEPTION_H
#define INC_1_CUSTOMEXCEPTION_H

#include <iostream>
#include <exception>
#include <utility>

#define ERROR_EXIT 1
#define NORMAL_EXIT 0


struct CustomException : public std::exception {
private:
    std::string error_message;
    int error_number;
public:
    CustomException(int errNumber, std::string& msg): error_number(errNumber), error_message(msg){}

    const char* what() const noexcept;
    int getErrorNumber() const noexcept;
};

#endif //INC_1_CUSTOMEXCEPTION_H
