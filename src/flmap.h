#ifndef FLMAP_H
#define FLMAP_H

#include <map>


template <typename T, typename B>
class FlMap : public std::map<T, B>
{
public:
    void insert(const T &key, const B &value);
    B take(const T &key);
    bool remove(const T &key);

    B value(const T &key, const B &defaultValue) const;
};


template <typename T, typename B>
inline void FlMap<T, B>::insert(const T &key, const B &value)
{
    std::map<T, B>::insert(std::pair<T, B>(key, value));
}

template <typename T, typename B>
inline B FlMap<T, B>::take(const T &key)
{
    typename FlMap<T, B>::iterator it = find(key);

    if (it == FlMap<T, B>::end())
        return B();

    const B &result = it->second;
    erase(it);

    return result;
}

template <typename T, typename B>
inline bool FlMap<T, B>::remove(const T &key)
{
    typename FlMap<T, B>::iterator it = FlMap<T, B>::find(key);

    if (it == FlMap<T, B>::end())
        return false;

    FlMap<T, B>::erase(it);
    return true;
}

template <typename T, typename B>
inline B FlMap<T, B>::value(const T &key, const B &defaultValue) const
{
    typename FlMap<T, B>::const_iterator it = FlMap<T, B>::find(key);
    return (it == FlMap<T, B>::end()) ? defaultValue : it->second;
}

#endif
