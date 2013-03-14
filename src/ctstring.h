#ifndef CTSTRING_H
#define CTSTRING_H

#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>

class CtString
{
public:
    CtString() {}
    CtString(const char *cstr) : mStr(cstr) {}
    CtString(const char *cstr, int len) : mStr(cstr, len) {}
    CtString(const std::string &str) : mStr(str) {}
    CtString(const CtString &str) : mStr(str.mStr) {}

    inline int lastIndexOf(char c) const;

    inline bool contains(const CtString &str) const {
        return mStr.find(str.mStr) != std::string::npos;
    }

    inline bool isEmpty() const {
        return mStr.empty();
    }

    inline size_t length() const {
        return mStr.length();
    }

    inline void remove(int index, int count) {
        mStr.erase(index, count);
    }

    inline void append(const char ch) {
        mStr.push_back(ch);
    }

    inline void append(const char *str) {
        mStr.append(str);
    }

    inline void append(const CtString &str) {
        mStr.append(str.mStr);
    }

    inline const char *data() const {
        return mStr.c_str();
    }

    inline char at(int index) const {
        return mStr.at(index);
    }

    inline int compare(const char *str) const {
        return mStr.compare(str);
    }

    inline int compare(const CtString &str) const {
        return mStr.compare(str.mStr);
    }

    inline void operator+=(const char *str) {
        mStr.append(str);
    }

    inline void operator+=(const CtString &str) {
        mStr.append(str.mStr);
    }

    inline char operator[](size_t index) const {
        return mStr[index];
    }

    inline CtString substr(size_t pos) const {
        return CtString(mStr.substr(pos, std::string::npos));
    }

    inline CtString substr(size_t pos, size_t len) const {
        return CtString(mStr.substr(pos, len));
    }

    static CtString format(const char *format, size_t size, ...);

private:
    std::string mStr;

    friend std::ostream &operator<<(std::ostream &os, const CtString &str);
};


inline CtString operator+(const CtString &str, const char *cstr)
{
    CtString result(str);
    result.append(cstr);
    return result;
}

inline CtString operator+(const char *cstr, const CtString &str)
{
    CtString result(cstr);
    result.append(str);
    return result;
}

inline CtString operator+(const CtString &str1, const CtString &str2)
{
    CtString result(str1);
    result.append(str2);
    return result;
}

inline std::ostream &operator<<(std::ostream &os, const CtString &str)
{
    os << str.mStr;
    return os;
}

inline bool operator==(const CtString &str, const char *cstr)
{
    return str.compare(cstr) == 0;
}

inline bool operator==(const char *cstr, const CtString &str)
{
    return str.compare(cstr) == 0;
}

inline bool operator==(const CtString str1, const CtString &str2)
{
    return str1.compare(str2) == 0;
}

inline bool operator!=(const CtString &str, const char *cstr)
{
    return str.compare(cstr) != 0;
}

inline bool operator!=(const char *cstr, const CtString &str)
{
    return str.compare(cstr) != 0;
}

inline bool operator!=(const CtString str1, const CtString &str2)
{
    return str1.compare(str2) != 0;
}

inline bool operator<(const CtString &str1, const CtString &str2)
{
    return str1.compare(str2) < 0;
}

inline bool operator>(const CtString &str1, const CtString &str2)
{
    return str1.compare(str2) > 0;
}


inline int CtString::lastIndexOf(char c) const
{
    const size_t idx = mStr.find_last_of(c);
    return (idx >= mStr.length()) ? -1 : idx;
}

inline CtString CtString::format(const char *format, size_t size, ...)
{
    char *buffer = new char[size];

    va_list args;
    va_start(args, size);
    vsprintf(buffer,format, args);
    va_end(args);

    CtString result(buffer);
    delete [] buffer;

    return result;
}

#endif
