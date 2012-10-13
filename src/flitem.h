#ifndef FLITEM_H
#define FLITEM_H

#include <stdlib.h>
#include "flGL.h"
#include "fllist.h"
#include "flevents.h"
#include "flmatrix.h"
#include "fltexture.h"

class FlDragCursor;
class FlSceneItem;
class FlSceneView;
class FlSceneViewData;
class FlSceneItemPrivate;
class FlShaderEffect;

class FlSceneItem
{
public:
    enum ChangeType {
        XChange,
        YChange,
        ZChange,
        WidthChange,
        HeightChange,
        RotationChange,
        XScaleChange,
        YScaleChange,
        OpacityChange,
        VisibilityChange
    };

    union ChangeValue {
        int intValue;
        bool boolValue;
        flreal realValue;
        FlSceneItem *itemValue;
    };

    enum Flag {
        NoFlag = 0x0,
        AcceptsMouseEvent = 0x1,
        AcceptsTouchEvent = 0x2,
        AcceptsDragEvent = 0x4,
    };

    FlSceneItem(FlSceneItem *parent = 0);
    virtual ~FlSceneItem();

    flreal x() const;
    void setX(flreal x);

    flreal y() const;
    void setY(flreal y);

    flreal z() const;
    void setZ(flreal z);

    flreal width() const;
    void setWidth(flreal width);

    flreal height() const;
    void setHeight(flreal width);

    flreal rotation() const;
    void setRotation(flreal rotation);

    flreal xScale() const;
    void setXScale(flreal scale);

    flreal yScale() const;
    void setYScale(flreal scale);

    void scale(flreal xScale, flreal yScale);

    flreal opacity() const;
    void setOpacity(flreal opacity);

    int flags() const;
    void setFlag(Flag flag, bool enabled);

    bool setDragCursor(FlDragCursor *drag);

    virtual bool contains(flreal x, flreal y);

    FlMatrix transformMatrix() const;
    FlMatrix sceneTransformMatrix() const;

    void setTransformOrigin(flreal x, flreal y);

    FlPointReal mapToScene(flreal x, flreal y) const;

    FlPointReal mapToItem(FlSceneItem *item, flreal x, flreal y) const;
    FlPointReal mapFromItem(FlSceneItem *item, flreal x, flreal y) const;

    FlSceneView *scene() const;
    FlSceneItem *parent() const;

    FlList<FlSceneItem *> children() const;

    bool isVisible() const;
    void setVisible(bool visible);

    virtual FlRectReal boundingRect() const;

    bool collidesWith(FlSceneItem *item) const;

protected:
    FlSceneItem(FlSceneItemPrivate *dd);

    virtual void paint();
    virtual void advance(fluint ms);
    virtual bool event(FlEvent *event);
    virtual void itemChanged(ChangeType, const ChangeValue &) {}

    virtual void mousePressEvent(FlMouseEvent *event);
    virtual void mouseMoveEvent(FlMouseEvent *event);
    virtual void mouseReleaseEvent(FlMouseEvent *event);

    virtual void touchBeginEvent(FlTouchEvent *event);
    virtual void touchUpdateEvent(FlTouchEvent *event);
    virtual void touchEndEvent(FlTouchEvent *event);

    virtual void dragEnterEvent(FlDragDropEvent *event);
    virtual void dragMoveEvent(FlDragDropEvent *event);
    virtual void dragLeaveEvent(FlDragDropEvent *event);
    virtual void dropEvent(FlDragDropEvent *event);

    virtual void dragCursorDropEvent(FlDragDropEvent *event);
    virtual void dragCursorCancelEvent(FlDragDropEvent *event);

    FlSceneItemPrivate *d_ptr;

private:
    friend class FlSceneView;
    friend class FlSceneViewData;
    friend class FlSceneItemPrivate;
};


class FlSceneRect : public FlSceneItem
{
public:
    FlSceneRect(FlSceneItem *parent = 0);
    FlSceneRect(flreal r, flreal g, flreal b, FlSceneItem *parent = 0);

    flreal r() const;
    flreal g() const;
    flreal b() const;
    void setColor(flreal r, flreal g, flreal b);

    FlShaderEffect *shaderEffect() const;
    void setShaderEffect(FlShaderEffect *effect);

protected:
    void paint();
};

class FlSceneImage : public FlSceneItem
{
public:
    enum FillMode {
        Stretch,
        Tile,
        TileVertically,
        TileHorizontally
    };

    FlSceneImage(FlSceneItem *parent = 0);
    FlSceneImage(FlTexture *texture, FlSceneItem *parent = 0);

    FillMode fillMode() const;
    void setFillMode(FillMode mode);

    FlTexture *texture() const;
    void setTexture(FlTexture *texture);

    int textureAtlasIndex() const;
    void setTextureAtlasIndex(int index);

    bool load(const FlString &filePath);

    bool load(const FlString &filePath, Fl::TextureFileType type);

    FlShaderEffect *shaderEffect() const;
    void setShaderEffect(FlShaderEffect *effect);

protected:
    void paint();
};

#endif
