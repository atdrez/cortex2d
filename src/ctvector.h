#ifndef CTVECTOR_H
#define CTVECTOR_H

#include <vector>

template <typename T>
class CtVector : public std::vector<T>
{
public:
    bool remove(const T &key);
};


template <typename T>
inline bool CtVector<T>::remove(const T &key)
{
    typename CtVector<T>::iterator it;

    for (it = CtVector<T>::begin(); it != CtVector<T>::end(); it++) {
        if (*it == key) {
            CtVector<T>::erase(it);
            return true;
        }
    }

    return false;
}


#endif
