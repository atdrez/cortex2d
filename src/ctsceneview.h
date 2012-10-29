#ifndef CTSCENEVIEW_H
#define CTSCENEVIEW_H

#include "ctwindow.h"

class CtSceneItem;
class CtDragCursor;
class CtSceneItemPrivate;

class CtSceneView : public CtWindow
{
public:
    CtSceneView(const char *title, int width, int height);
    virtual ~CtSceneView();

    void setRootItem(CtSceneItem *item);

protected:
    void paint();
    void advance(ctuint ms);
    bool event(CtEvent *event);

private:
    bool setDragCursor(CtDragCursor *drag);
    void itemAddedToScene(CtSceneItem *item);
    void itemRemovedFromScene(CtSceneItem *item);
    void itemZValueChanged(CtSceneItem *item);

    friend class CtSceneItem;
    friend class CtSceneItemPrivate;
};

#endif
