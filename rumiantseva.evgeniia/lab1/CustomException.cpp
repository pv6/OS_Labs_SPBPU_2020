//
// Created by Evgenia on 26.10.2020.
//

#include "CustomException.h"

const char* CustomException::what () const noexcept {
    return error_message.c_str();
}

int CustomException::getErrorNumber() const noexcept {
    return error_number;
}