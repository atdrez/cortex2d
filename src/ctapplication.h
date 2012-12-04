#ifndef CTAPPLICATION_H
#define CTAPPLICATION_H

#include "ctglobal.h"
#include "ctobject.h"

class CtWindow;
class CtApplicationPrivate;

class CtApplication : public CtObject
{
public:
    CtApplication(int argc, char **argv);
    virtual ~CtApplication();

    void quit();

    CtString applicationDir() const;
    CtString applicationPath() const;

    static CtApplication *instance();

    static void openURL(const CtString &url);

protected:
    bool event(CtEvent *event);
    virtual void readyEvent(CtEvent *event);
    virtual void releaseEvent(CtEvent *event);

private:
    CtApplicationPrivate *d_ptr;

    friend class CtWindow;
    friend class CtWindowPrivate;
    friend class CtApplicationPrivate;
    friend int ctMain(int argc, char **argv, CtApplication *app);

};

int ctMain(int argc, char **argv, CtApplication *app);

#endif
