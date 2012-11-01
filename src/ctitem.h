#ifndef CTITEM_H
#define CTITEM_H

#include <stdlib.h>
#include "ctGL.h"
#include "ctlist.h"
#include "ctevents.h"
#include "ctmatrix.h"
#include "cttexture.h"

class CtDragCursor;
class CtSceneItem;
class CtSceneView;
class CtSceneViewData;
class CtSceneItemPrivate;
class CtShaderEffect;
class CtSceneImagePrivate;

class CtSceneItem
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
        ctreal realValue;
        CtSceneItem *itemValue;
    };

    enum Flag {
        NoFlag = 0x0,
        AcceptsMouseEvent = 0x1,
        AcceptsTouchEvent = 0x2,
        AcceptsDragEvent = 0x4,
    };

    CtSceneItem(CtSceneItem *parent = 0);
    virtual ~CtSceneItem();

    ctreal x() const;
    void setX(ctreal x);

    ctreal y() const;
    void setY(ctreal y);

    ctreal z() const;
    void setZ(ctreal z);

    ctreal width() const;
    void setWidth(ctreal width);

    ctreal height() const;
    void setHeight(ctreal width);

    ctreal rotation() const;
    void setRotation(ctreal rotation);

    ctreal xScale() const;
    void setXScale(ctreal scale);

    ctreal yScale() const;
    void setYScale(ctreal scale);

    void scale(ctreal xScale, ctreal yScale);

    ctreal opacity() const;
    void setOpacity(ctreal opacity);

    int flags() const;
    void setFlag(Flag flag, bool enabled);

    bool setDragCursor(CtDragCursor *drag);

    virtual bool contains(ctreal x, ctreal y);

    CtMatrix transformMatrix() const;
    CtMatrix sceneTransformMatrix() const;

    void setTransformOrigin(ctreal x, ctreal y);

    CtPointReal mapToScene(ctreal x, ctreal y) const;

    CtPointReal mapToItem(CtSceneItem *item, ctreal x, ctreal y) const;
    CtPointReal mapFromItem(CtSceneItem *item, ctreal x, ctreal y) const;

    CtSceneView *scene() const;
    CtSceneItem *parent() const;

    CtList<CtSceneItem *> children() const;

    bool isVisible() const;
    void setVisible(bool visible);

    void deleteLater();

    virtual CtRectReal boundingRect() const;

    bool collidesWith(CtSceneItem *item) const;

protected:
    CtSceneItem(CtSceneItemPrivate *dd);

    virtual void paint();
    virtual void advance(ctuint ms);
    virtual bool event(CtEvent *event);
    virtual void itemChanged(ChangeType, const ChangeValue &) {}

    virtual void mousePressEvent(CtMouseEvent *event);
    virtual void mouseMoveEvent(CtMouseEvent *event);
    virtual void mouseReleaseEvent(CtMouseEvent *event);

    virtual void touchBeginEvent(CtTouchEvent *event);
    virtual void touchUpdateEvent(CtTouchEvent *event);
    virtual void touchEndEvent(CtTouchEvent *event);

    virtual void dragEnterEvent(CtDragDropEvent *event);
    virtual void dragMoveEvent(CtDragDropEvent *event);
    virtual void dragLeaveEvent(CtDragDropEvent *event);
    virtual void dropEvent(CtDragDropEvent *event);

    virtual void dragCursorDropEvent(CtDragDropEvent *event);
    virtual void dragCursorCancelEvent(CtDragDropEvent *event);

    CtSceneItemPrivate *d_ptr;

private:
    friend class CtSceneView;
    friend class CtSceneViewData;
    friend class CtSceneItemPrivate;
};


class CtSceneRect : public CtSceneItem
{
public:
    CtSceneRect(CtSceneItem *parent = 0);
    CtSceneRect(ctreal r, ctreal g, ctreal b, CtSceneItem *parent = 0);

    ctreal r() const;
    ctreal g() const;
    ctreal b() const;
    void setColor(ctreal r, ctreal g, ctreal b);

    CtShaderEffect *shaderEffect() const;
    void setShaderEffect(CtShaderEffect *effect);

protected:
    void paint();
};

class CtSceneImage : public CtSceneItem
{
public:
    enum FillMode {
        Stretch,
        Tile,
        TileVertically,
        TileHorizontally
    };

    CtSceneImage(CtSceneItem *parent = 0);
    CtSceneImage(CtTexture *texture, CtSceneItem *parent = 0);

    FillMode fillMode() const;
    void setFillMode(FillMode mode);

    CtTexture *texture() const;
    void setTexture(CtTexture *texture);

    int textureAtlasIndex() const;
    void setTextureAtlasIndex(int index);

    bool load(const CtString &filePath);

    bool load(const CtString &filePath, Ct::TextureFileType type);

    CtShaderEffect *shaderEffect() const;
    void setShaderEffect(CtShaderEffect *effect);

protected:
    CtSceneImage(CtSceneImagePrivate *dd);

    void paint();
};


class CtSceneFragments : public CtSceneImage
{
public:
    class Fragment {
    public:
        Fragment();

        ctreal x() const { return m_x; }
        void setX(ctreal x);

        ctreal y() const { return m_y; }
        void setY(ctreal y);

        ctreal width() const { return m_width; }
        void setWidth(ctreal w);

        ctreal height() const { return m_height; }
        void setHeight(ctreal h);

        int atlasIndex() const { return m_atlasIndex; }
        void setAtlasIndex(int index);

        void *userData() const { return m_userData; }
        void setUserData(void *data);

    private:
        ctreal m_x;
        ctreal m_y;
        ctreal m_width;
        ctreal m_height;
        int m_atlasIndex;
        void *m_userData;
    };

    CtSceneFragments(CtSceneItem *parent = 0);
    CtSceneFragments(CtTexture *texture, CtSceneItem *parent = 0);

    CtList<Fragment *> fragments() const;

    void clearFragments();
    bool appendFragment(Fragment *fragment);
    bool insertFragment(int index, Fragment *fragment);
    bool removeFragment(Fragment *fragment);

protected:
    void paint();
};

#endif
