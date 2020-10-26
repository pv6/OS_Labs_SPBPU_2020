//
// Created by Daria on 10/22/2020.
//

#ifndef DAEMON_LAB1_ERROR_H
#define DAEMON_LAB1_ERROR_H

#include <string>
class error {
public:
    typedef enum error_name {
        WRONG_CONFIG, WRONG_FILE, NOT_OPENED, PARSER_ERROR, DIR_ERROR, PID_ERROR, OK
    }error_name;
    static char* print_error(error_name name) {
        char* error_str;
        switch (name) {
            case WRONG_CONFIG:
                error_str = (char*)"Incorrect structure of config";
                break;
            case WRONG_FILE:
                error_str = (char*)"No such config file";
                break;
            case NOT_OPENED:
                error_str = (char*)"Cannot open file";
                break;
            case PARSER_ERROR:
                error_str = (char*)"Error in parser:";
                break;
            case DIR_ERROR:
                error_str = (char*)"Error in directory path";
                break;
            case PID_ERROR:
                error_str = (char*)"Error in pid file";
                break;
            default:
                error_str = (char*)"";
        }
        return error_str;
    }
};

#endif //DAEMON_LAB1_ERROR_H
