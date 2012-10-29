#ifndef CTSTRING_H
#define CTSTRING_H

#include <string>

class CtString : public std::string
{
public:
    CtString() : std::string() {}
    CtString(const char *c) : std::string(c) {}
    CtString(const char *c, int len) : std::string(c, len) {}
    CtString(const CtString &c) : std::string(c) {}
    CtString(const std::basic_string<char> &c) : std::string(c) {}

    inline bool contains(const CtString &s) const {
        return find(s) != std::string::npos;
    }
};

#endif
