#ifndef FLSTRING_H
#define FLSTRING_H

#include <string>

class FlString : public std::string
{
public:
    FlString() : std::string() {}
    FlString(const char *c) : std::string(c) {}
    FlString(const char *c, int len) : std::string(c, len) {}
    FlString(const FlString &c) : std::string(c) {}
    FlString(const std::basic_string<char> &c) : std::string(c) {}

    inline bool contains(const FlString &s) const {
        return find(s) != std::string::npos;
    }
};

#endif
