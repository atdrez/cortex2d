#ifndef CTITEM_H
#define CTITEM_H

#include <stdlib.h>
#include "ctGL.h"
#include "ctlist.h"
#include "ctevents.h"
#include "ctmatrix.h"
#include "cttexture.h"
#include "ctobject.h"
#include "ctcolor.h"


class CtDragCursor;
class CtSceneItem;
class CtSceneView;
class CtSceneViewData;
class CtSceneItemPrivate;
class CtShaderEffect;
class CtSceneImagePrivate;
class CtSceneTextureItemPrivate;
class CtRenderer;
class CtTextureFont;


class CtSceneItem : public CtObject
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
        VisibilityChange,
        ImplicitWidthChange,
        ImplicitHeightChange,
        FrozenChange,
        ChildAdded,
        ChildRemoved
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
        IgnoreParentOpacity = 0x8,
        IgnoreAllParentOpacity = 0x10,
        DoNotPaintContent = 0x20
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

    ctreal implicitWidth() const;
    void setImplicitWidth(ctreal width);

    ctreal implicitHeight() const;
    void setImplicitHeight(ctreal height);

    void resize(ctreal width, ctreal height);

    ctreal rotation() const;
    void setRotation(ctreal rotation);

    ctreal xScale() const;
    void setXScale(ctreal scale);

    ctreal yScale() const;
    void setYScale(ctreal scale);

    bool isFrozen() const;
    void setFrozen(bool frozen);

    void scale(ctreal xScale, ctreal yScale);

    ctreal opacity() const;
    void setOpacity(ctreal opacity);

    int flags() const;
    void setFlag(Flag flag, bool enabled);

    bool setDragCursor(CtDragCursor *drag);

    virtual bool contains(ctreal x, ctreal y);

    CtMatrix transformMatrix() const;
    CtMatrix sceneTransformMatrix() const;

    void setLocalTransform(const CtMatrix &matrix);

    void translate(ctreal x, ctreal y);

    CtPoint transformOrigin() const;
    void setTransformOrigin(ctreal x, ctreal y);

    CtPoint mapToScene(ctreal x, ctreal y) const;

    CtPoint mapToItem(CtSceneItem *item, ctreal x, ctreal y) const;
    CtPoint mapFromItem(CtSceneItem *item, ctreal x, ctreal y) const;

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

    virtual void paint(CtRenderer *renderer);
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
    friend class CtSceneFrameBufferPrivate;
};


class CtSceneRect : public CtSceneItem
{
public:
    CtSceneRect(CtSceneItem *parent = 0);
    CtSceneRect(ctreal r, ctreal g, ctreal b, CtSceneItem *parent = 0);

    CtColor color() const;
    void setColor(const CtColor &color);

    CtShaderEffect *shaderEffect() const;
    void setShaderEffect(CtShaderEffect *effect);

protected:
    void paint(CtRenderer *renderer);
};


class CtSceneText : public CtSceneItem
{
public:
    CtSceneText(CtSceneItem *parent = 0);

    CtColor color() const;
    void setColor(const CtColor &color);

    CtString text() const;
    void setText(const CtString &text);

    CtTextureFont *font() const;
    void setFont(CtTextureFont *font);

    CtShaderEffect *shaderEffect() const;
    void setShaderEffect(CtShaderEffect *effect);

protected:
    void paint(CtRenderer *renderer);
};


class CtSceneFrameBuffer : public CtSceneItem
{
public:
    CtSceneFrameBuffer(CtSceneItem *parent = 0);

    CtShaderEffect *shaderEffect() const;
    void setShaderEffect(CtShaderEffect *effect);

    bool isValidBuffer() const;

    void setBufferSize(int width, int height);

protected:
    void paint(CtRenderer *renderer);
};


class CtSceneTextureItem : public CtSceneItem
{
public:
    CtSceneTextureItem(CtSceneItem *parent = 0);
    CtSceneTextureItem(CtTexture *texture, CtSceneItem *parent = 0);

    CtTexture *texture() const;
    void setTexture(CtTexture *texture);

    int textureAtlasIndex() const;
    void setTextureAtlasIndex(int index);

    bool load(const CtString &filePath);

    CtShaderEffect *shaderEffect() const;
    void setShaderEffect(CtShaderEffect *effect);

protected:
    CtSceneTextureItem(CtSceneTextureItemPrivate *dd);
};


class CtSceneImage : public CtSceneTextureItem
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

protected:
    CtSceneImage(CtSceneImagePrivate *dd);

    void paint(CtRenderer *renderer);
};


class CtSceneImagePoly : public CtSceneImage
{
public:
    CtSceneImagePoly(CtSceneItem *parent = 0);
    CtSceneImagePoly(CtTexture *texture, CtSceneItem *parent = 0);

    CtVector<CtPoint> vertices() const;
    void setVertices(const CtVector<CtPoint> &vertices);

protected:
    void paint(CtRenderer *renderer);
};


class CtSceneFragments : public CtSceneTextureItem
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

        ctreal opacity() const { return m_opacity; }
        void setOpacity(ctreal opacity) { m_opacity = opacity; }

        void *userData() const { return m_userData; }
        void setUserData(void *data);

    private:
        ctreal m_x;
        ctreal m_y;
        ctreal m_width;
        ctreal m_height;
        ctreal m_opacity;
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
    void paint(CtRenderer *renderer);
};


class CtSceneParticleSystem : public CtSceneTextureItem
{
public:
    class Particle {
    public:
        Particle();

        ctreal x() const { return m_x; }
        void setX(ctreal x) { m_x = x; }

        ctreal y() const { return m_y; }
        void setY(ctreal y) { m_y = y; }

        ctreal pointSize() const { return m_size; }
        void setPointSize(ctreal size) { m_size = size; }

        CtColor color() const { return m_color; }
        void setColor(const CtColor &color) { m_color = color; }

        void *userData() const { return m_userData; }
        void setUserData(void *data) { m_userData = data; }

    private:
        ctreal m_x;
        ctreal m_y;
        ctreal m_size;
        CtColor m_color;
        void *m_userData;
    };

    CtSceneParticleSystem(CtSceneItem *parent = 0);
    CtSceneParticleSystem(CtTexture *texture, CtSceneItem *parent = 0);

    CtVector<Particle *> particles() const;

    bool addParticle(Particle *);
    bool removeParticle(Particle *);
    void clearParticles();

protected:
    void paint(CtRenderer *renderer);
};


#endif
