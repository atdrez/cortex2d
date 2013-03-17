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
class CtSprite;
class CtSceneView;
class CtSceneViewData;
class CtShaderEffect;
class CtRenderer;
class CtFont;
class CtFrameBufferSprite;

class CtSprite : public CtObject
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
        CtSprite *itemValue;
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

    CtSprite(CtSprite *parent = 0);
    virtual ~CtSprite();

    inline ctreal x() const { return mX; }
    void setX(ctreal x);

    inline ctreal y() const { return mY; }
    void setY(ctreal y);

    inline ctreal z() const { return mZ; }
    void setZ(ctreal z);

    inline ctreal width() const { return mWidth; }
    void setWidth(ctreal width);

    inline ctreal height() const { return mHeight; }
    void setHeight(ctreal width);

    inline ctreal implicitWidth() const { return mImplicitWidth; }
    void setImplicitWidth(ctreal width);

    inline ctreal implicitHeight() const { return mImplicitHeight; }
    void setImplicitHeight(ctreal height);

    void resize(ctreal width, ctreal height);

    inline ctreal rotation() const { return mRotation; }
    void setRotation(ctreal rotation);

    inline ctreal xScale() const { return mXScale; }
    void setXScale(ctreal scale);

    inline ctreal yScale() const { return mYScale; }
    void setYScale(ctreal scale);

    inline bool isFrozen() const { return mIsFrozen; }
    void setFrozen(bool frozen);

    void scale(ctreal xScale, ctreal yScale);

    ctreal opacity() const;
    void setOpacity(ctreal opacity);

    inline int flags() const { return mFlags; }
    void setFlag(Flag flag, bool enabled);

    bool setDragCursor(CtDragCursor *drag);

    virtual bool contains(ctreal x, ctreal y) const;

    CtMatrix transformMatrix() const;
    CtMatrix sceneTransformMatrix() const;

    void setLocalTransform(const CtMatrix &matrix);

    void translate(ctreal x, ctreal y);

    CtPoint transformOrigin() const;
    void setTransformOrigin(ctreal x, ctreal y);

    CtPoint mapToScene(ctreal x, ctreal y) const;

    CtPoint mapToItem(CtSprite *item, ctreal x, ctreal y) const;
    CtPoint mapFromItem(CtSprite *item, ctreal x, ctreal y) const;

    CtSceneView *scene() const;
    CtSprite *parent() const { return mParent; }

    CtList<CtSprite *> children() const;

    bool isVisible() const;
    void setVisible(bool visible);

    void deleteLater();

    virtual CtRect boundingRect() const;

    bool collidesWith(CtSprite *item) const;

protected:
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

protected:
    void addItem(CtSprite *item);
    void removeItem(CtSprite *item);
    void setScene(CtSceneView *newScene);
    void setIsFrameBuffer(bool isFrameBuffer) { mIsFrameBuffer = isFrameBuffer; }

    CtFrameBufferSprite *frameBufferItem() const;
    CtMatrix4x4 currentViewportProjectionMatrix();

    const CtList<CtSprite *> &orderedChildren();
    virtual void recursivePaint(CtRenderer *state);

private:
    void checkTransformMatrix() const;
    CtMatrix mappedTransformMatrix(CtSprite *root) const;

    bool relativeVisible() const;
    ctreal relativeOpacity() const;
    bool relativeFrozen() const;

    void fillItems(CtList<CtSprite *> &lst);

    ctreal mX;
    ctreal mY;
    ctreal mZ;
    ctreal mWidth;
    ctreal mHeight;
    ctreal mXScale;
    ctreal mYScale;
    ctreal mRotation;
    ctreal mOpacity;
    bool mVisible;
    bool mIsFrozen;
    ctreal mImplicitWidth;
    ctreal mImplicitHeight;
    CtSprite *mParent;
    CtSceneView *mScene;
    ctreal mXCenter;
    ctreal mYCenter;
    bool mSortDirty;
    mutable bool mTransformDirty;
    int mFlags;
    bool mIsFrameBuffer;
    bool mPendingDelete;
    CtMatrix mLocalMatrix;
    mutable CtMatrix mFboTransformMatrix;
    mutable CtMatrix mLocalTransformMatrix;
    mutable CtMatrix mSceneTransformMatrix;

    CtList<CtSprite *> mChildren;
    CtList<CtSprite *> mSortedChildren;

    friend class CtSceneView;
    friend class CtSceneViewData;
    friend class CtFrameBufferSprite;
};


class CtRectSprite : public CtSprite
{
public:
    CtRectSprite(CtSprite *parent = 0);
    CtRectSprite(ctreal r, ctreal g, ctreal b, CtSprite *parent = 0);

    inline CtColor color() const { return mColor; }
    void setColor(const CtColor &color);

    CtShaderEffect *shaderEffect() const { return mShaderEffect; }
    void setShaderEffect(CtShaderEffect *effect);

protected:
    void paint(CtRenderer *renderer);

private:
    CtColor mColor;
    CtShaderEffect *mShaderEffect;
};


class CtTextSprite : public CtSprite
{
public:
    CtTextSprite(CtSprite *parent = 0);
    ~CtTextSprite();

    inline CtColor color() const { return mColor; }
    void setColor(const CtColor &color);

    inline CtString text() const { return mText; }
    void setText(const CtString &text);

    inline CtFont *font() const { return mFont; }
    void setFont(CtFont *font);

    inline CtShaderEffect *shaderEffect() const { return mShaderEffect; }
    void setShaderEffect(CtShaderEffect *effect);

protected:
    void paint(CtRenderer *renderer);

private:
    void releaseBuffers();
    void recreateBuffers();

    CtColor mColor;
    CtString mText;
    int mGlyphCount;
    GLuint mIndexBuffer;
    GLuint mVertexBuffer;
    CtFont *mFont;
    CtShaderEffect *mShaderEffect;
};


class CtFrameBufferSprite : public CtSprite
{
public:
    CtFrameBufferSprite(CtSprite *parent = 0);
    ~CtFrameBufferSprite();

    CtShaderEffect *shaderEffect() const { return mShaderEffect; }
    void setShaderEffect(CtShaderEffect *effect);

    bool isValidBuffer() const;

    void setBufferSize(int width, int height);

protected:
    void paint(CtRenderer *renderer);
    void deleteBuffers();
    void resizeBuffer(int w, int h);

    virtual void recursivePaint(CtRenderer *state);

private:
    int mBufferWidth;
    int mBufferHeight;
    GLuint mFramebuffer;
    GLuint mDepthbuffer;
    CtTexture *mTexture;
    CtShaderEffect *mShaderEffect;
};


class CtTextureSprite : public CtSprite
{
public:
    CtTextureSprite(CtSprite *parent = 0);
    CtTextureSprite(CtTexture *texture, CtSprite *parent = 0);
    ~CtTextureSprite();

    CtTexture *texture() const { return mTexture; }
    void setTexture(CtTexture *texture);

    int textureAtlasIndex() const { return mTextureAtlasIndex; }
    void setTextureAtlasIndex(int index);

    bool load(const CtString &filePath);

    CtShaderEffect *shaderEffect() const { return mShaderEffect; }
    void setShaderEffect(CtShaderEffect *effect);

private:
    void releaseTexture();

    int mTextureAtlasIndex;
    bool mOwnTexture;
    CtTexture *mTexture;
    CtShaderEffect *mShaderEffect;
};


class CtImageSprite : public CtTextureSprite
{
public:
    enum FillMode {
        Stretch,
        Tile,
        TileVertically,
        TileHorizontally
    };

    CtImageSprite(CtSprite *parent = 0);
    CtImageSprite(CtTexture *texture, CtSprite *parent = 0);

    FillMode fillMode() const { return mFillMode; }
    void setFillMode(FillMode mode) { mFillMode = mode; }

protected:
    void paint(CtRenderer *renderer);

private:
    CtImageSprite::FillMode mFillMode;
};


class CtImagePolygonSprite : public CtImageSprite
{
public:
    CtImagePolygonSprite(CtSprite *parent = 0);
    CtImagePolygonSprite(CtTexture *texture, CtSprite *parent = 0);
    ~CtImagePolygonSprite();

    CtVector<CtPoint> vertices() const { return mVertices; }
    void setVertices(const CtVector<CtPoint> &vertices) { mVertices = vertices; }

protected:
    void paint(CtRenderer *renderer);

private:
    CtVector<CtPoint> mVertices;
};


class CtFragmentsSprite : public CtTextureSprite
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

    CtFragmentsSprite(CtSprite *parent = 0);
    CtFragmentsSprite(CtTexture *texture, CtSprite *parent = 0);
    ~CtFragmentsSprite();

    CtList<Fragment *> fragments() const { return mFragments; }

    void clearFragments();
    bool appendFragment(Fragment *fragment);
    bool insertFragment(int index, Fragment *fragment);
    bool removeFragment(Fragment *fragment);

protected:
    void paint(CtRenderer *renderer);

private:
    CtList<CtFragmentsSprite::Fragment *> mFragments;
};


class CtParticlesSprite : public CtTextureSprite
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

    CtParticlesSprite(CtSprite *parent = 0);
    CtParticlesSprite(CtTexture *texture, CtSprite *parent = 0);
    ~CtParticlesSprite();

    CtVector<Particle *> particles() const { return mParticles; }

    bool addParticle(Particle *);
    bool removeParticle(Particle *);
    void clearParticles();

protected:
    void paint(CtRenderer *renderer);

private:
    void recreateVertexBuffer();

    GLfloat *mVertices;
    int mAttrCount;
    int mVertexSize;
    int mVertexCount;
    CtVector<CtParticlesSprite::Particle *> mParticles;
};


#endif
