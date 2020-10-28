#include "Helpers.h"
#include <cstring>

void Helpers::strToCStr(char** dest, std::string& src) {
    if (*dest) {
        delete [] (*dest);
        *dest = nullptr;
    }
    (*dest) = new char[src.length() + 1];
    strcpy(*dest, src.c_str());
}

void Helpers::copyCStr(char** dest, char const * const src) {
    if (*dest) {
        delete [] (*dest);
        *dest = nullptr;
    }
    (*dest) = new char[strlen(src) + 1];
    strcpy(*dest, src);
}
