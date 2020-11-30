#include <stdio.h>
#include <string>

inline void printOk(const char *str) {
    printf("[\033[0;32mOK\033[0m] %s\n", str);
}
inline void printOk(const char *str, int id) {
    printf("[\033[0;32mConnection %i: OK\033[0m] %s\n", id, str);
}

inline void printOk(const std::string &str) {
    printf("[\033[0;32mOK\033[0m] %s\n", str.c_str());
}
inline void printOk(const std::string &str, int id) {
    printf("[\033[0;32mConnection %i: OK\033[0m] %s\n", id, str.c_str());
}

inline void printErr(const char *str) {
    printf("[\033[0;31mERROR\033[0m] %s\n", str);
}
inline void printErr(const char *str, int id) {
    printf("[\033[0;31mConnection %i: ERROR\033[0m] %s\n", id, str);
}

inline void printErr(const std::string &str) {
    printf("[\033[0;31mERROR\033[0m] %s\n", str.c_str());
}
inline void printErr(const std::string &str, int id) {
    printf("[\033[0;31mConnection %i: ERROR\033[0m] %s\n", id, str.c_str());
}