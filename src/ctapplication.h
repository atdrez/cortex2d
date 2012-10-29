#ifndef CTAPPLICATION_H
#define CTAPPLICATION_H

#include "ctglobal.h"

class CtWindow;
class CtApplicationPrivate;

class CtApplication
{
public:
    CtApplication(int argc, char **argv);
    virtual ~CtApplication();

    int exec();
    void quit();

    CtString applicationDir() const;
    CtString applicationPath() const;

    static CtApplication *instance();

private:
    CtApplicationPrivate *d_ptr;

    friend class CtWindow;
    friend class CtWindowPrivate;
    friend class CtApplicationPrivate;
};

#endif
