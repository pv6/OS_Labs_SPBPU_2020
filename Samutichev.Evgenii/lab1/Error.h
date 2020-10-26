#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

enum class Error {
    OK = 0,
    NO_SUCH_FIELD = 1,
    NO_CONFIG_FILE = 2,
    CONFIG_FAIL = 3,
    NO_SUCH_FOLDER = 4,
    UNKNOWN = 5
};

#endif // ERROR_H_INCLUDED
