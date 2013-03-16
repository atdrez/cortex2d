#ifndef CTPOOL_H
#define CTPOOL_H

#include "ctglobal.h"
#include "ctstring.h"


template<typename T>
class CtPool
{
public:
    static bool contains(const CtString &id);

    static T *value(const CtString &id);
    static void insert(const CtString &id, T *value);
    static void remove(const CtString &id);
    static T *take(const CtString &id);

    static void clear();

private:
    CtPool() {}
    ~CtPool() {}

    static CtPool *instance();

    CtMap<CtString, T *> mValues;
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
inline void CtPool<T>::insert(const CtString &id, T *value)
{
    instance()->mValues.insert(id, value);
}

template <typename T>
inline void CtPool<T>::remove(const CtString &id)
{
    T *value = CtPool<T>::take(id);

    if (value)
        delete value;
}

template <typename T>
inline T *CtPool<T>::value(const CtString &id)
{
    return instance()->mValues.value(id, 0);
}

template <typename T>
inline void CtPool<T>::clear()
{
    CtPool<T> *d = instance();
    typename CtMap<CtString, T *>::iterator it;

    for (it = d->mValues.begin(); it != d->mValues.end(); it++)
        delete it->second;

    d->mValues.clear();
}

template <typename T>
inline T *CtPool<T>::take(const CtString &id)
{
    CtPool<T> *d = instance();
    typename CtMap<CtString, T *>::iterator it = d->mValues.find(id);

    if (it == d->mValues.end())
        return 0;

    T *result = it->second;
    d->mValues.remove(it);

    return result;
}

#endif
