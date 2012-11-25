#ifndef CTBASICAPPLICATION_H
#define CTBASICAPPLICATION_H

#include "ctapplication.h"
#include "ctwindow.h"

template <class T>
class CtBasicApplication : public CtApplication
{
public:
    CtBasicApplication(int argc, char **argv)
        : CtApplication(argc, argv),
          m_window(0)
    {

    }

protected:
    void readyEvent(CtEvent *) {
        m_window = new T();

        if (m_window)
            m_window->show();
    }

    void releaseEvent(CtEvent *) {
        delete m_window;
        m_window = 0;
    }

private:
    CtWindow *m_window;
};

#endif
