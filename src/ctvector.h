#ifndef CTVECTOR_H
#define CTVECTOR_H

#include <vector>
#include <stdio.h>

template <typename T>
class CtVector
{
public:
    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;
    typedef typename std::vector<T>::reverse_iterator reverse_iterator;
    typedef typename std::vector<T>::const_reverse_iterator const_reverse_iterator;

    CtVector() {}

    inline iterator begin() { return mData.begin(); }
    inline const_iterator begin() const { return mData.begin(); }

    inline iterator end() { return mData.end(); }
    inline const_iterator end() const { return mData.end(); }

    inline reverse_iterator rbegin() { return mData.rbegin(); }
    inline const_reverse_iterator rbegin() const { return mData.rbegin(); }

    inline reverse_iterator rend() { return mData.rend(); }
    inline const_reverse_iterator rend() const { return mData.rend(); }

    inline void clear() { mData.clear(); }
    inline size_t size() const { return mData.size(); }
    inline bool isEmpty() const { return mData.empty(); }

    inline void append(const T &value) { mData.push_back(value); }
    inline void insert(iterator it, const T &value) { mData.insert(it, value); }

    inline bool remove(const T &key);
    inline void remove(iterator it) { mData.erase(it); }

    inline T &front() { return mData.front(); }
    inline const T &front() const { return mData.front(); }

    inline T &back() { return mData.back(); }
    inline const T &back() const { return mData.back(); }

    inline void resize(size_t size) { mData.resize(size); }

    inline T &at(size_t index) { return mData.at(index); }
    const T &at(size_t index) const { return mData.at(index); }

    T &operator[](size_t index) { return mData.at(index); }
    const T &operator[](size_t index) const { return mData.at(index); }

private:
    std::vector<T> mData;
};


template <typename T>
inline bool CtVector<T>::remove(const T &key)
{
    typename CtVector<T>::iterator it;

    for (it = mData.begin(); it != mData.end(); it++) {
        if (*it == key) {
            mData.erase(it);
            return true;
        }
    }

    return false;
}


#endif
