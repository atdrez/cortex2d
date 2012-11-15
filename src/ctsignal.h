#ifndef CTSIGNAL_H
#define CTSIGNAL_H

#include <set>

namespace CtPrivate {
    /**************************************************************************
     * Callback0
     **************************************************************************/
    template<class P = void>
    struct AbstractCallback0
    {
        virtual void emit() = 0;
        virtual bool match(AbstractCallback0 *cb) = 0;
    };

    template<class T>
    struct Callback0 : public AbstractCallback0<>
    {
        typedef void (T::*Method)();

        Callback0(T *obj, Method m)
            : object(obj), method(m) {}

        void emit() {
            if (object && method)
                (object->*method)();
        }

        bool match(AbstractCallback0 *c) {
            if (!c)
                return false;
            Callback0 *r = reinterpret_cast<Callback0 *>(c);
            return (r->object == object && r->method == method);
        }

        T *object;
        Method method;
    };

    /**************************************************************************
     * Callback1
     **************************************************************************/
    template<class P1>
    struct AbstractCallback1
    {
        typedef AbstractCallback1<P1> AbsCb;

        virtual void emit(P1 p1) = 0;
        virtual bool match(AbsCb *cb) = 0;
    };

    template<class T, class P1>
    struct Callback1 : public AbstractCallback1<P1>
    {
        typedef void (T::*Method)(P1 p1);

        Callback1(T *obj, Method m)
            : object(obj), method(m) {}

        void emit(P1 p1) {
            if (object && method)
                (object->*method)(p1);
        }

        bool match(AbstractCallback1<P1> *c) {
            if (!c)
                return false;
            Callback1<T, P1> *r = reinterpret_cast<Callback1<T, P1> *>(c);
            return (r->object == object && r->method == method);
        }

        T *object;
        Method method;
    };

    template<class Cb>
    void removeCallback(typename std::set<Cb *> *callbacks, Cb *callback)
    {
        typename std::set<Cb *>::const_iterator i;
        for (i = callbacks->begin(); i != callbacks->end(); i++) {
            if ((*i)->match(callback)) {
                delete *i;
                callbacks->erase(i);
                return;
            }
        }
    }
}

/**************************************************************************
 * Signal0
 **************************************************************************/

class CtSignal0
{
private:
    typedef CtPrivate::AbstractCallback0<void> _Callback;

public:
    template<class T>
    void connect(T *obj, void (T::*func)()) {
        m_callbacks.insert(new CtPrivate::Callback0<T>(obj, func));
    }

    template<class T>
    void connect(T *obj, void (T::*func)() const) {
        m_callbacks.insert(new CtPrivate::Callback0<T>(obj, func));
    }

    template<class T>
    void disconnect(T *obj, void (T::*func)()) {
        CtPrivate::Callback0<T> callback(obj, func);
        removeCallback(&m_callbacks, reinterpret_cast<_Callback *>(&callback));
    }

    template< class T>
    void disconnect(T * obj, void (T::*func)() const) {
        CtPrivate::Callback0<T> callback(obj, func);
        removeCallback(&m_callbacks, reinterpret_cast<_Callback *>(&callback));
    }

    void operator()() {
        std::set<_Callback *>::const_iterator i;
        for (i = m_callbacks.begin(); i != m_callbacks.end(); i++)
            (*i)->emit();
    }

    void clear() {
        m_callbacks.clear();
    }

    bool isEmpty() const {
        return m_callbacks.empty();
    }

private:
    std::set<_Callback *> m_callbacks;
};


/**************************************************************************
 * Signal1
 **************************************************************************/

template<class P1>
class CtSignal1
{
private:
    typedef CtPrivate::AbstractCallback1<P1> _Callback;

public:
    template<class T>
    void connect(T *obj, void (T::*func)(P1 p1)) {
        m_callbacks.insert(new CtPrivate::Callback1<T, P1>(obj, func));
    }

    template<class T>
    void connect(T *obj, void (T::*func)(P1 p1) const) {
        m_callbacks.insert(new CtPrivate::Callback1<T, P1>(obj, func));
    }

    template<class T>
    void disconnect(T *obj, void (T::*func)(P1 p1)) {
        CtPrivate::Callback1<T, P1> callback(obj, func);
        removeCallback(&m_callbacks, reinterpret_cast<_Callback *>(&callback));
    }

    template< class T>
    void disconnect(T * obj, void (T::*func)(P1 p1) const) {
        CtPrivate::Callback1<T, P1> callback(obj, func);
        removeCallback(&m_callbacks, reinterpret_cast<_Callback *>(&callback));
    }

    void operator()(P1 p1) {
        typename std::set<_Callback *>::const_iterator i;
        for (i = m_callbacks.begin(); i != m_callbacks.end(); i++)
            (*i)->emit(p1);
    }

    void clear() {
        m_callbacks.clear();
    }

    bool isEmpty() const {
        return m_callbacks.empty();
    }

private:
    std::set<_Callback *> m_callbacks;
};

#endif
