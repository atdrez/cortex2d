#ifndef CTMAP_H
#define CTMAP_H

#include <map>


template <typename T, typename B>
class CtMap
{
public:
    typedef typename std::map<T, B>::iterator iterator;
    typedef typename std::map<T, B>::const_iterator const_iterator;
    typedef typename std::map<T, B>::reverse_iterator reverse_iterator;
    typedef typename std::map<T, B>::const_reverse_iterator const_reverse_iterator;

    CtMap() {}

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

    inline void insert(const T &key, const B &value) {
        mData.insert(std::pair<T, B>(key, value));
    }

    inline B take(const T &key);
    inline bool remove(const T &key);

    inline void remove(iterator it) { mData.erase(it); }

    inline bool contains(const T &key) {
        return mData.find(key) != mData.end();
    }

    inline B value(const T &key, const B &defaultValue) const;

    inline B &operator[](const T &key) {
        return mData[key];
    }

    inline B operator[](const T &key) const {
        return mData[key];
    }

    inline iterator find(const T &key) { return mData.find(key); }

private:
    std::map<T, B> mData;
};


template <typename T, typename B>
inline B CtMap<T, B>::take(const T &key)
{
    typename CtMap<T, B>::iterator it = mData.find(key);

    if (it == mData.end())
        return B();

    const B &result = it->second;
    mData.erase(it);

    return result;
}

template <typename T, typename B>
inline bool CtMap<T, B>::remove(const T &key)
{
    typename CtMap<T, B>::iterator it = mData.find(key);

    if (it == mData.end())
        return false;

    mData.erase(it);
    return true;
}

template <typename T, typename B>
inline B CtMap<T, B>::value(const T &key, const B &defaultValue) const
{
    typename CtMap<T, B>::const_iterator it = mData.find(key);
    return (it == mData.end()) ? defaultValue : it->second;
}

#endif
