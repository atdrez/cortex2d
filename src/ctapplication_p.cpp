#include <string.h>
#include "ctapplication_p.h"
#include "ctwindow_p.h"


CtApplicationPrivate::CtApplicationPrivate(CtApplication *q)
    : q_ptr(q),
      argc(0),
      argv(0)
{

}

bool CtApplicationPrivate::init(int argc, char **argv)
{
    this->argc = argc;

    if (argc <= 0)
        this->argv = 0;
    else {
        this->argv = new char*[argc];

        for (int i = 0; i < argc; i++) {
            const int len = strlen(argv[i]);
            this->argv[i] = new char[len + 1];
            strcpy(this->argv[i], argv[i]);
        }
    }

    return true;
}

void CtApplicationPrivate::postEvent(CtWindowPrivate *window, CtEvent *event)
{
    if (window)
        window->q_ptr->event(event);
}

void CtApplicationPrivate::addWindow(CtWindowPrivate *window)
{
    windows.append(window);
}

void CtApplicationPrivate::removeWindow(CtWindowPrivate *window)
{
    windows.removeAll(window);
}

void CtApplicationPrivate::quit()
{
    while (!windows.isEmpty()) {
        CtWindowPrivate *window = windows.front();
        window->release();
    }
}

void CtApplicationPrivate::postEventToAllWindows(CtEvent *event)
{
    foreach (CtWindowPrivate *window, windows)
        postEvent(window, event);
}
