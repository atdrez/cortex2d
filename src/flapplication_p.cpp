#include <string.h>
#include "flapplication_p.h"
#include "flwindow_p.h"


FlApplicationPrivate::FlApplicationPrivate(FlApplication *q)
    : q_ptr(q),
      argc(0),
      argv(0)
{

}

bool FlApplicationPrivate::init(int argc, char **argv)
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

void FlApplicationPrivate::postEvent(FlWindowPrivate *window, FlEvent *event)
{
    if (window)
        window->q_ptr->event(event);
}

void FlApplicationPrivate::addWindow(FlWindowPrivate *window)
{
    windows.push_back(window);
}

void FlApplicationPrivate::removeWindow(FlWindowPrivate *window)
{
    windows.remove(window);
}

void FlApplicationPrivate::quit()
{
    while (!windows.empty()) {
        FlWindowPrivate *window = windows.front();
        window->release();
    }
}
