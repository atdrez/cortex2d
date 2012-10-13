#ifndef FLFUNCTOR_H
#define FLFUNCTOR_H

class FlFunctor
{
public:
    void operator()(void) { call(); }
    void operator()(int v) { call(v); }
    void operator()(flreal v) {call(v); }

protected:
    virtual void call() {}
    virtual void call(int) {}
    virtual void call(flreal) {}
};

template <class T, typename P>
class FlCallback : public FlFunctor
{
public:
    FlCallback()
        : m_obj(0), m_func(0) {}

    inline void bind(T *obj, void(T::*func)(P value)) {
        m_obj = obj; m_func = func;
    }

    inline void unbind() {
        m_obj = 0; m_func = 0;
    }

protected:
    inline void call(P value) {
        if (m_obj && m_func)
            (*m_obj.*m_func)(value);
    }

private:
    T *m_obj;
    void (T::*m_func)(P value);
};

#endif
