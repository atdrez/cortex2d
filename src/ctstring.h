#ifndef CTSTRING_H
#define CTSTRING_H

#include <string>
#include <stdio.h>
#include <stdarg.h>

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

    static CtString format(const char *format, size_t size, ...) {
        char *buffer = new char[size];

        va_list args;
        va_start(args, size);
        vsprintf(buffer,format, args);
        va_end(args);

        CtString result(buffer);
        delete buffer;

        return result;
    }
};

#endif
