#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED

#include <string>

namespace Helpers {
    void strToCStr(char** dest, std::string& src);
    void copyCStr(char** dest, char const * const src);
};

#endif // HELPERS_H_INCLUDED
