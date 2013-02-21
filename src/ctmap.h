#ifndef CTMAP_H
#define CTMAP_H

#include <map>


template <typename T, typename B>
class CtMap : public std::map<T, B>
{
public:
    void insert(const T &key, const B &value);
    B take(const T &key);
    bool remove(const T &key);

    B value(const T &key, const B &defaultValue) const;
};


template <typename T, typename B>
inline void CtMap<T, B>::insert(const T &key, const B &value)
{
    std::map<T, B>::insert(std::pair<T, B>(key, value));
}

template <typename T, typename B>
inline B CtMap<T, B>::take(const T &key)
{
    typename CtMap<T, B>::iterator it = CtMap<T, B>::find(key);

    if (it == CtMap<T, B>::end())
        return B();

    const B &result = it->second;
    CtMap<T, B>::erase(it);

    return result;
}

template <typename T, typename B>
inline bool CtMap<T, B>::remove(const T &key)
{
    typename CtMap<T, B>::iterator it = CtMap<T, B>::find(key);

    if (it == CtMap<T, B>::end())
        return false;

    CtMap<T, B>::erase(it);
    return true;
}

template <typename T, typename B>
inline B CtMap<T, B>::value(const T &key, const B &defaultValue) const
{
    typename CtMap<T, B>::const_iterator it = CtMap<T, B>::find(key);
    return (it == CtMap<T, B>::end()) ? defaultValue : it->second;
}

#endif
