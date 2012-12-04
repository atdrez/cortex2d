#ifndef CTLIST_H
#define CTLIST_H

#include <list>

template <typename T>
class CtList : public std::list<T>
{
public:
    bool contains(const T &value) const;
};

template <typename T>
inline bool CtList<T>::contains(const T &value) const
{
    typename CtList<T>::const_iterator it;

    for (it = CtList<T>::begin(); it != CtList<T>::end(); it++) {
        if (*it == value)
            return true;
    }

    return false;
}

#endif
