#ifndef CTPOOL_H
#define CTPOOL_H

#include "ctglobal.h"
#include "ctstring.h"


template<typename T>
class CtPool
{
public:
    static bool contains(const CtString &id);

    static void insert(const CtString &id, const T &value);
    static void remove(const CtString &id);

    static T value(const CtString &id, const T &defaultValue = T());

private:
    CtPool() {}
    ~CtPool() {}

    static CtPool *instance();

    CtMap<CtString, T> mValues;
};


template <typename T>
CtPool<T> *CtPool<T>::instance()
{
    static CtPool *result = new CtPool();
    return result;
}

template <typename T>
inline bool CtPool<T>::contains(const CtString &id)
{
    return instance()->mValues.contains(id);
}

template <typename T>
inline void CtPool<T>::insert(const CtString &id, const T &value)
{
    instance()->mValues.insert(id, value);
}

template <typename T>
inline void CtPool<T>::remove(const CtString &id)
{
    instance()->mValues.remove(id);
}

template <typename T>
inline T CtPool<T>::value(const CtString &id, const T &defaultValue)
{
    return instance()->mValues.value(id, defaultValue);
}

#endif
