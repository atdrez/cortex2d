#ifndef FLSCENEVIEW_H
#define FLSCENEVIEW_H

#include "flwindow.h"

class FlSceneItem;
class FlDragCursor;
class FlSceneItemPrivate;

class FlSceneView : public FlWindow
{
public:
    FlSceneView(const char *title, int width, int height);
    virtual ~FlSceneView();

    void setRootItem(FlSceneItem *item);

protected:
    void paint();
    void advance(fluint ms);
    bool event(FlEvent *event);

private:
    bool setDragCursor(FlDragCursor *drag);
    void itemAddedToScene(FlSceneItem *item);
    void itemRemovedFromScene(FlSceneItem *item);
    void itemZValueChanged(FlSceneItem *item);

    friend class FlSceneItem;
    friend class FlSceneItemPrivate;
};

#endif
