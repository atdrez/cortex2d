#ifndef CTLIST_H
#define CTLIST_H

#include <list>
#include <stdio.h>


template <typename T>
class CtList
{
public:
    typedef typename std::list<T>::iterator iterator;
    typedef typename std::list<T>::const_iterator const_iterator;
    typedef typename std::list<T>::reverse_iterator reverse_iterator;
    typedef typename std::list<T>::const_reverse_iterator const_reverse_iterator;

    CtList() : mList() {}

    inline iterator begin() { return mList.begin(); }
    inline const_iterator begin() const { return mList.begin(); }

    inline iterator end() { return mList.end(); }
    inline const_iterator end() const { return mList.end(); }

    inline reverse_iterator rbegin() { return mList.rbegin(); }
    inline const_reverse_iterator rbegin() const { return mList.rbegin(); }

    inline reverse_iterator rend() { return mList.rend(); }
    inline const_reverse_iterator rend() const { return mList.rend(); }

    bool contains(const T &value) const;

    inline void clear() { mList.clear(); }
    inline size_t size() const { return mList.size(); }
    inline bool isEmpty() const { return mList.empty(); }

    inline void append(const T &value) { mList.push_back(value); }

    inline void removeFirst() { mList.pop_front(); }
    inline void removeLast() { mList.pop_back(); }
    inline void removeAll(const T &value) { mList.remove(value); }

    inline T &front() { return mList.front(); }
    inline const T &front() const { return mList.front(); }

    inline T &back() { return mList.back(); }
    inline const T &back() const { return mList.back(); }

    inline iterator remove(iterator it) { return mList.erase(it); }
    inline iterator insert(iterator it, const T &value) { return mList.insert(it, value); }

    template <class Compare>
    inline void sort(Compare comp) { mList.sort(comp); }

private:
    std::list<T> mList;
};


template <typename T>
bool CtList<T>::contains(const T &value) const
{
    typename std::list<T>::const_iterator it;

    for (it = mList.begin(); it != mList.end(); it++) {
        if (*it == value)
            return true;
    }

    return false;
}

#endif
