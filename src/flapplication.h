#ifndef FLAPPLICATION_H
#define FLAPPLICATION_H

#include "flglobal.h"

class FlWindow;
class FlApplicationPrivate;

class FlApplication
{
public:
    FlApplication(int argc, char **argv);
    virtual ~FlApplication();

    int exec();
    void quit();

    FlString applicationDir() const;
    FlString applicationPath() const;

    static FlApplication *instance();

private:
    FlApplicationPrivate *d_ptr;

    friend class FlWindow;
    friend class FlWindowPrivate;
    friend class FlApplicationPrivate;
};

#endif
