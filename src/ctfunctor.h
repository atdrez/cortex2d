#ifndef CTFUNCTOR_H
#define CTFUNCTOR_H

class CtFunctor
{
public:
    void operator()(void) { call(); }
    void operator()(int v) { call(v); }
    void operator()(ctreal v) {call(v); }

protected:
    virtual void call() {}
    virtual void call(int) {}
    virtual void call(ctreal) {}
};

template <class T, typename P>
class CtCallback : public CtFunctor
{
public:
    CtCallback()
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
