#ifndef CTITEM_H
#define CTITEM_H

#include <stdlib.h>
#include "ctGL.h"
#include "ctlist.h"
#include "ctglobal.h"
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

    bool isFrozen() const;
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

    ctreal relativeOpacity() const;

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

    CT_PRIVATE_COPY(CtSprite);
};


class CtRectSprite : public CtSprite
{
public:
    CtRectSprite(CtSprite *parent = 0);
    CtRectSprite(ctreal r, ctreal g, ctreal b, CtSprite *parent = 0);
    ~CtRectSprite();

    inline CtColor color() const { return mColor; }
    void setColor(const CtColor &color);

    CtShaderEffect *shaderEffect() const { return mShaderEffect; }
    void setShaderEffect(CtShaderEffect *effect);

protected:
    void paint(CtRenderer *renderer);

private:
    CtColor mColor;
    CtShaderEffect *mShaderEffect;

    CT_PRIVATE_COPY(CtRectSprite);
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

    CT_PRIVATE_COPY(CtTextSprite);
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

    CT_PRIVATE_COPY(CtFrameBufferSprite);
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

    CT_PRIVATE_COPY(CtTextureSprite);
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
    ~CtImageSprite();

    FillMode fillMode() const { return mFillMode; }
    void setFillMode(FillMode mode) { mFillMode = mode; }

protected:
    void paint(CtRenderer *renderer);

private:
    CtImageSprite::FillMode mFillMode;

    CT_PRIVATE_COPY(CtImageSprite);
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

    CT_PRIVATE_COPY(CtImagePolygonSprite);
};


class CtFragmentsSprite : public CtTextureSprite
{
public:
    class Fragment {
    public:
        Fragment();

        inline ctreal x() const { return mX; }
        void setX(ctreal x) { mX = x; }

        inline ctreal y() const { return mY; }
        void setY(ctreal y) { mY = y; }

        inline ctreal width() const { return mWidth; }
        void setWidth(ctreal w) { mWidth = w; }

        inline ctreal height() const { return mHeight; }
        void setHeight(ctreal h) { mHeight = h; }

        inline int atlasIndex() const { return mAtlasIndex; }
        void setAtlasIndex(int index) { mAtlasIndex = index; }

        inline ctreal opacity() const { return mOpacity; }
        void setOpacity(ctreal opacity) { mOpacity = opacity; }

        inline void *userData() const { return mUserData; }
        void setUserData(void *data) { mUserData = data; }

    private:
        ctreal mX;
        ctreal mY;
        ctreal mWidth;
        ctreal mHeight;
        ctreal mOpacity;
        int mAtlasIndex;
        void *mUserData;
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

    CT_PRIVATE_COPY(CtFragmentsSprite);
};


class CtParticlesSprite : public CtTextureSprite
{
public:
    class Particle {
    public:
        Particle();

        inline ctreal x() const { return mX; }
        inline void setX(ctreal x) { mX = x; }

        inline ctreal y() const { return mY; }
        inline void setY(ctreal y) { mY = y; }

        inline ctreal pointSize() const { return mSize; }
        inline void setPointSize(ctreal size) { mSize = size; }

        inline CtColor color() const { return mColor; }
        inline void setColor(const CtColor &color) { mColor = color; }

        inline void *userData() const { return mUserData; }
        inline void setUserData(void *data) { mUserData = data; }

    private:
        ctreal mX;
        ctreal mY;
        ctreal mSize;
        CtColor mColor;
        void *mUserData;
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

    CT_PRIVATE_COPY(CtParticlesSprite);
};


#endif
