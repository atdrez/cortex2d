#ifndef CTBASICAPPLICATION_H
#define CTBASICAPPLICATION_H

#include "ctapplication.h"
#include "ctglobal.h"
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
        setActiveWindow(m_window);

        if (m_window)
            m_window->show();
    }

    void releaseEvent(CtEvent *) {
        if (m_window == activeWindow())
            setActiveWindow(0);

        delete m_window;
        m_window = 0;
    }

private:
    CtWindow *m_window;

    CT_PRIVATE_COPY(CtBasicApplication);
};

#endif
