#ifndef CTSCENEVIEW_H
#define CTSCENEVIEW_H

#include "ctwindow.h"

class CtSprite;
class CtDragCursor;
class CtSpritePrivate;

class CtSceneView : public CtWindow
{
public:
    CtSceneView(const char *title, int width, int height);
    virtual ~CtSceneView();

    void setRootItem(CtSprite *item);

protected:
    void paint();
    void advance(ctuint ms);
    bool event(CtEvent *event);

private:
    bool setDragCursor(CtDragCursor *drag);
    void itemAddedToScene(CtSprite *item);
    void itemRemovedFromScene(CtSprite *item);
    void itemZValueChanged(CtSprite *item);

    friend class CtSprite;
    friend class CtSpritePrivate;

    CT_PRIVATE_COPY(CtSceneView);
};

#endif
