#include "ctsprite.h"
#include "ctmath.h"
#include "ctfont.h"
#include "ctrenderer.h"
#include "ctsceneview.h"
#include "ctdragcursor.h"
#include "ctshadereffect.h"
#include "ctopenglfunctions.h"
#include "3rdparty/tricollision.h"
#include "freetype-gl.h"


static CtShaderEffect *ct_sharedSolidShaderEffect()
{
    static CtShaderEffect *r =
        new CtShaderEffect(CtGpuProgram::sharedSolidShaderProgram());

    return r;
}

static CtShaderEffect *ct_sharedTextureShaderEffect()
{
    static CtShaderEffect *r =
        new CtShaderEffect(CtGpuProgram::sharedTextureShaderProgram());
    return r;
}

static CtShaderEffect *ct_sharedTextShaderEffect()
{
    static CtShaderEffect *r
        = new CtShaderEffect(CtGpuProgram::sharedTextShaderProgram());
    return r;
}

static CtShaderEffect *ct_sharedFragmentShaderEffect()
{
    static CtShaderEffect *r
        = new CtShaderEffect(CtGpuProgram::sharedFragmentShaderProgram());
    return r;
}

static CtShaderEffect *ct_sharedParticleShaderEffect()
{
    static CtShaderEffect *r
        = new CtShaderEffect(CtGpuProgram::sharedParticleShaderProgram());
    return r;
}

bool CtSprite_sort_compare(CtSprite *a, CtSprite *b)
{
    return (!a || !b) ? false : (a->z() < b->z());
}


ctreal CtSprite::relativeOpacity() const
{
    // XXX: optimize
    if (!mParent || mOpacity == 0.0 || (mFlags & CtSprite::IgnoreAllParentOpacity)) {
        return mOpacity;
    } else if ((mFlags & CtSprite::IgnoreParentOpacity)) {
        CtSprite *topParent = mParent ? mParent->parent() : 0;

        if (!topParent)
            return mOpacity;
        else
            return mOpacity * topParent->opacity();
    } else {
        return mOpacity * mParent->opacity();
    }
}

CtFrameBufferSprite *CtSprite::frameBufferItem() const
{
    // XXX: optimize
    if (!mParent)
        return 0;
    else if (mParent->mIsFrameBuffer)
        return static_cast<CtFrameBufferSprite *>(mParent);
    else
        return mParent->frameBufferItem();
}

void CtSprite::fillItems(CtList<CtSprite *> &lst)
{
    lst.append(this);

    const CtList<CtSprite *> &items = orderedChildren();

    foreach (CtSprite *item, items)
        item->fillItems(lst);
}

const CtList<CtSprite *> &CtSprite::orderedChildren()
{
    if (!mSortDirty)
        return mSortedChildren;

    mSortDirty = false;
    mSortedChildren = mChildren;
    mSortedChildren.sort(CtSprite_sort_compare);

    return mSortedChildren;
}

void CtSprite::recursivePaint(CtRenderer *renderer)
{
    if ((mFlags & CtSprite::DoNotPaintContent) == 0) {
        renderer->begin();
        renderer->m_opacity = relativeOpacity();
        renderer->m_projectionMatrix = currentViewportProjectionMatrix();
        paint(renderer);
        renderer->end();
    }

    const CtList<CtSprite *> &lst = orderedChildren();

    foreach (CtSprite *item, lst) {
        if (item->isVisible())
            item->recursivePaint(renderer);
    }
}

void CtSprite::addItem(CtSprite *item)
{
    if (mChildren.contains(item))
        return;

    mChildren.removeAll(item);
    mChildren.append(item);

    mSortDirty = true;

    CtSceneView *mScene = scene();

    if (mScene)
        mScene->itemAddedToScene(this);

    CtSprite::ChangeValue value;
    value.itemValue = item;
    itemChanged(CtSprite::ChildAdded, value);
}

void CtSprite::removeItem(CtSprite *item)
{
    if (!mChildren.contains(item))
        return;

    mChildren.removeAll(item);

    mSortDirty = true;

    CtSceneView *mScene = scene();

    if (mScene)
        mScene->itemRemovedFromScene(this);

    CtSprite::ChangeValue value;
    value.itemValue = item;
    itemChanged(CtSprite::ChildRemoved, value);
}

void CtSprite::setScene(CtSceneView *newScene)
{
    mScene = newScene;
    // update cache
}

CtMatrix CtSprite::mappedTransformMatrix(CtSprite *root) const
{
    const ctreal ox = ctRound(mXCenter);
    const ctreal oy = ctRound(mYCenter);

    // pixel aligned
    const ctreal rx = ctRound(mX);
    const ctreal ry = ctRound(mY);

    CtMatrix modelMatrix;
    modelMatrix.translate(rx, ry);

    modelMatrix.translate(ox, oy);
    modelMatrix.scale(mXScale, mYScale);
    modelMatrix.rotate(-(GLfloat)mRotation);
    modelMatrix.translate(-ox, -oy);

    CtMatrix m = mLocalMatrix;
    m.multiply(modelMatrix);
    modelMatrix.setMatrix(m);

    if (mParent && (mParent != root))
        modelMatrix.multiply(mParent->mappedTransformMatrix(root));

    if (!root) {
        mSceneTransformMatrix = modelMatrix;
        mLocalTransformMatrix = modelMatrix;
        mLocalTransformMatrix.invert();
    }

    return modelMatrix;
}

void CtSprite::checkTransformMatrix() const
{
    if (!mTransformDirty)
        return;

    mSceneTransformMatrix = mappedTransformMatrix(0);
    mLocalTransformMatrix = mSceneTransformMatrix;
    mLocalTransformMatrix.invert();

    // fbo
    CtFrameBufferSprite *fbo = frameBufferItem();

    if (!fbo || !fbo->isValidBuffer())
        mFboTransformMatrix = mSceneTransformMatrix;
    else
        mFboTransformMatrix = mappedTransformMatrix(fbo);

    //transformDirty = true; // XXX: enable for optimization
}

CtMatrix4x4 CtSprite::currentViewportProjectionMatrix()
{
    CtSceneView *sc = scene();
    CtFrameBufferSprite *fb = frameBufferItem();

    checkTransformMatrix();

    CtMatrix4x4 result;

    if (sc) {
        if (!fb || !fb->isValidBuffer())
            result = mSceneTransformMatrix.toMatrix4x4();
        else
            result = mFboTransformMatrix.toMatrix4x4();

        CtMatrix4x4 ortho;
        ortho.ortho(0, sc->width(), sc->height(), 0, 1, -1);
        result.multiply(ortho);
    }

    return result;
}

CtSprite::CtSprite(CtSprite *parent)
    : CtObject(parent),
      mX(0),
      mY(0),
      mZ(0),
      mWidth(0),
      mHeight(0),
      mXScale(1),
      mYScale(1),
      mRotation(0),
      mOpacity(1.0),
      mVisible(true),
      mIsFrozen(false),
      mImplicitWidth(0),
      mImplicitHeight(0),
      mParent(parent),
      mScene(0),
      mXCenter(0),
      mYCenter(0),
      mSortDirty(false),
      mTransformDirty(true),
      mFlags(CtSprite::NoFlag),
      mIsFrameBuffer(false),
      mPendingDelete(false),
      mLocalMatrix(),
      mFboTransformMatrix(),
      mLocalTransformMatrix(),
      mSceneTransformMatrix(),
      mChildren(),
      mSortedChildren()
{
    if (mParent)
        mParent->addItem(this);
}

CtSprite::~CtSprite()
{
    if (mParent)
        mParent->removeItem(this);

    // delete children
    CtList<CtSprite *> oldChildren = mChildren;

    foreach (CtSprite *item, oldChildren)
        delete item;
}

CtSceneView *CtSprite::scene() const
{
    if (mParent)
        return mParent->scene();
    else
        return mScene;
}

void CtSprite::setX(ctreal x)
{
    if (mX == x)
        return;

    mX = x;

    ChangeValue value;
    value.realValue = x;
    itemChanged(XChange, value);
}

void CtSprite::setY(ctreal y)
{
    if (mY == y)
        return;

    mY = y;

    ChangeValue value;
    value.realValue = y;
    itemChanged(YChange, value);
}

void CtSprite::setZ(ctreal z)
{
    if (mZ == z)
        return;

    mZ = z;

    ChangeValue value;
    value.realValue = z;
    itemChanged(ZChange, value);
}

void CtSprite::setRotation(ctreal rotation)
{
    if (mRotation == rotation)
        return;

    mRotation = rotation;

    ChangeValue value;
    value.realValue = rotation;
    itemChanged(RotationChange, value);
}

void CtSprite::setFrozen(bool frozen)
{
    if (mIsFrozen == frozen)
        return;

    mIsFrozen = frozen;

    // XXX: should emit when parent changes too
    ChangeValue value;
    value.boolValue = frozen;
    itemChanged(FrozenChange, value);
}

void CtSprite::setXScale(ctreal scale)
{
    if (mXScale == scale)
        return;

    mXScale = scale;

    ChangeValue value;
    value.realValue = scale;
    itemChanged(XScaleChange, value);
}

void CtSprite::setYScale(ctreal scale)
{
    if (mYScale == scale)
        return;

    mYScale = scale;

    ChangeValue value;
    value.realValue = scale;
    itemChanged(YScaleChange, value);
}

void CtSprite::scale(ctreal xScale, ctreal yScale)
{
    setXScale(xScale);
    setYScale(yScale);
}

ctreal CtSprite::opacity() const
{
    return relativeOpacity();
}

void CtSprite::setOpacity(ctreal opacity)
{
    opacity = ctClamp<ctreal>(0, opacity, 1);

    if (mOpacity == opacity)
        return;

    mOpacity = opacity;

    ChangeValue value;
    value.realValue = opacity;
    itemChanged(OpacityChange, value);
}

void CtSprite::setFlag(Flag flag, bool enabled)
{
    if (enabled) {
        mFlags |= flag;
    } else {
        if ((mFlags & flag))
            mFlags ^= flag;
    }
}

void CtSprite::deleteLater()
{
    mPendingDelete = true;
}

void CtSprite::setWidth(ctreal width)
{
    if (mWidth == width)
        return;

    mWidth = width;

    ChangeValue value;
    value.realValue = width;
    itemChanged(WidthChange, value);
}

void CtSprite::setHeight(ctreal height)
{
    if (mHeight == height)
        return;

    mHeight = height;

    ChangeValue value;
    value.realValue = height;
    itemChanged(HeightChange, value);
}

void CtSprite::setImplicitWidth(ctreal width)
{
    if (mImplicitWidth == width)
        return;

    mImplicitWidth = width;

    ChangeValue value;
    value.realValue = width;
    itemChanged(ImplicitWidthChange, value);
}

void CtSprite::setImplicitHeight(ctreal height)
{
    if (mImplicitHeight == height)
        return;

    mImplicitHeight = height;

    ChangeValue value;
    value.realValue = height;
    itemChanged(ImplicitHeightChange, value);
}

void CtSprite::resize(ctreal width, ctreal height)
{
    setWidth(width);
    setHeight(height);
}

bool CtSprite::contains(ctreal x, ctreal y) const
{
    return (x >= 0 && y >= 0 && x < mWidth && y < mHeight);
}

void CtSprite::paint(CtRenderer *renderer)
{
    CT_UNUSED(renderer);
}

void CtSprite::advance(ctuint ms)
{
    CT_UNUSED(ms);
}

bool CtSprite::isFrozen() const
{
    // XXX: optimize
    if (!mParent)
        return mIsFrozen;
    else
        return mIsFrozen || mParent->isFrozen();
}

bool CtSprite::isVisible() const
{
    // XXX: optimize
    if (!mParent)
        return mVisible;
    else
        return mVisible && mParent->isVisible();
}

void CtSprite::setVisible(bool visible)
{
    if (mVisible == visible)
        return;

    mVisible = visible;

    // XXX: should emit when parent changes too
    ChangeValue value;
    value.boolValue = visible;
    itemChanged(VisibilityChange, value);
}

void CtSprite::translate(ctreal x, ctreal y)
{
    mLocalMatrix.translate(x, y);
}

CtRect CtSprite::boundingRect() const
{
    return CtRect(0, 0, mWidth, mHeight);
}

bool CtSprite::setDragCursor(CtDragCursor *drag)
{
    if (!drag || drag->sourceItem() != this)
        return false;

    CtSceneView *itemScene = scene();
    return !itemScene ? false : itemScene->setDragCursor(drag);
}

CtPoint CtSprite::mapToScene(ctreal x, ctreal y) const
{
    return sceneTransformMatrix().map(x, y);
}

CtPoint CtSprite::mapToItem(CtSprite *item, ctreal x, ctreal y) const
{
    if (!item)
        return CtPoint();

    return item->transformMatrix().map(sceneTransformMatrix().map(x, y));
}

CtPoint CtSprite::mapFromItem(CtSprite *item, ctreal x, ctreal y) const
{
    if (!item)
        return CtPoint();

    return transformMatrix().map(item->sceneTransformMatrix().map(x, y));
}

bool CtSprite::collidesWith(CtSprite *item) const
{
    if (!item)
        return false;

    // bounding rects
    const CtRect &rA = boundingRect();
    const CtRect &rB = item->boundingRect();

    // matrix transforms
    const CtMatrix &mA = sceneTransformMatrix();
    const CtMatrix &mB = item->sceneTransformMatrix();

    // map top-left
    const CtPoint &a1 = mA.map(rA.x(), rA.y());
    const CtPoint &b1 = mB.map(rB.x(), rB.y());

    // map bottom-right
    const CtPoint &a3 = mA.map(rA.x() + rA.width(), rA.y() + rA.height());
    const CtPoint &b3 = mB.map(rB.x() + rB.width(), rB.y() + rB.height());

    // check simple collisions (no rotation)
    if (rotation() == 0 && item->rotation() == 0) {
        return !(ctMin(a1.x(), a3.x()) > ctMax(b1.x(), b3.x()) ||
                 ctMin(a1.y(), a3.y()) > ctMax(b1.y(), b3.y()) ||
                 ctMax(a1.x(), a3.x()) < ctMin(b1.x(), b3.x()) ||
                 ctMax(a1.y(), a3.y()) < ctMin(b1.y(), b3.y()));
    }

    // map top-right
    const CtPoint &a2 = mA.map(rA.x() + rA.width(), rA.y());
    const CtPoint &b2 = mB.map(rB.x() + rB.width(), rB.y());

    // map bottom-left
    const CtPoint &a4 = mA.map(rA.x(), rA.y() + rA.height());
    const CtPoint &b4 = mB.map(rB.x(), rB.y() + rB.height());

    // create triangles
    const float V[][3] = {{a1.x(), a1.y(), 0}, {a2.x(), a2.y(), 0}, {a3.x(), a3.y(), 0},
                          {a1.x(), a1.y(), 0}, {a3.x(), a3.y(), 0}, {a4.x(), a4.y(), 0}};

    const float U[][3] = {{b1.x(), b1.y(), 0}, {b2.x(), b2.y(), 0}, {b3.x(), b3.y(), 0},
                           {b1.x(), b1.y(), 0}, {b3.x(), b3.y(), 0}, {b4.x(), b4.y(), 0}};

    // check collisions
    if (tri_tri_intersect(V[0], V[1], V[2], U[0], U[1], U[2])) return true;
    if (tri_tri_intersect(V[0], V[1], V[2], U[3], U[4], U[5])) return true;
    if (tri_tri_intersect(V[3], V[4], V[5], U[0], U[1], U[2])) return true;
    if (tri_tri_intersect(V[3], V[4], V[5], U[3], U[4], U[5])) return true;

    return false;
}

CtPoint CtSprite::transformOrigin() const
{
    return CtPoint(mXCenter, mYCenter);
}

void CtSprite::setTransformOrigin(ctreal x, ctreal y)
{
    mXCenter = x;
    mYCenter = y;
}

CtList<CtSprite *> CtSprite::children() const
{
    return mChildren;
}

CtMatrix CtSprite::transformMatrix() const
{
    checkTransformMatrix();
    return mLocalTransformMatrix;
}

CtMatrix CtSprite::sceneTransformMatrix() const
{
    checkTransformMatrix();
    return mSceneTransformMatrix;
}

void CtSprite::setLocalTransform(const CtMatrix &matrix)
{
    mLocalMatrix = matrix;
}

bool CtSprite::event(CtEvent *event)
{
    event->setAccepted(true);

    switch (event->type()) {
    case CtEvent::MousePress:
        mousePressEvent(static_cast<CtMouseEvent *>(event));
        break;
    case CtEvent::MouseMove:
        mouseMoveEvent(static_cast<CtMouseEvent *>(event));
        break;
    case CtEvent::MouseRelease:
        mouseReleaseEvent(static_cast<CtMouseEvent *>(event));
        break;
    case CtEvent::TouchBegin:
        touchBeginEvent(static_cast<CtTouchEvent *>(event));
        break;
    case CtEvent::TouchUpdate:
        touchUpdateEvent(static_cast<CtTouchEvent *>(event));
        break;
    case CtEvent::TouchEnd:
        touchEndEvent(static_cast<CtTouchEvent *>(event));
        break;
    case CtEvent::DragEnter:
        dragEnterEvent(static_cast<CtDragDropEvent *>(event));
        break;
    case CtEvent::DragMove:
        dragMoveEvent(static_cast<CtDragDropEvent *>(event));
        break;
    case CtEvent::DragLeave:
        dragLeaveEvent(static_cast<CtDragDropEvent *>(event));
        break;
    case CtEvent::Drop:
        dropEvent(static_cast<CtDragDropEvent *>(event));
        break;
    case CtEvent::DragCursorDrop:
        dragCursorDropEvent(static_cast<CtDragDropEvent *>(event));
        break;
    case CtEvent::DragCursorCancel:
        dragCursorCancelEvent(static_cast<CtDragDropEvent *>(event));
        break;
    default:
        return CtObject::event(event);
    }

    return event->isAccepted();
}

void CtSprite::mousePressEvent(CtMouseEvent *event)
{
    event->setAccepted(false);
}

void CtSprite::mouseMoveEvent(CtMouseEvent *event)
{
    event->setAccepted(false);
}

void CtSprite::mouseReleaseEvent(CtMouseEvent *event)
{
    event->setAccepted(false);
}

void CtSprite::touchBeginEvent(CtTouchEvent *event)
{
    event->setAccepted(false);
}

void CtSprite::touchUpdateEvent(CtTouchEvent *event)
{
    event->setAccepted(false);
}

void CtSprite::touchEndEvent(CtTouchEvent *event)
{
    event->setAccepted(false);
}

void CtSprite::dragEnterEvent(CtDragDropEvent *event)
{
    event->setAccepted(false);
}

void CtSprite::dragMoveEvent(CtDragDropEvent *event)
{
    event->setAccepted(false);
}

void CtSprite::dragLeaveEvent(CtDragDropEvent *event)
{
    event->setAccepted(false);
}

void CtSprite::dropEvent(CtDragDropEvent *event)
{
    event->setAccepted(false);
}

void CtSprite::dragCursorDropEvent(CtDragDropEvent *event)
{
    event->setAccepted(false);
}

void CtSprite::dragCursorCancelEvent(CtDragDropEvent *event)
{
    event->setAccepted(false);
}

/////////////////////////////////////////////////
// CtRectSprite
/////////////////////////////////////////////////

CtRectSprite::CtRectSprite(CtSprite *parent)
    : CtSprite(parent),
      mColor(1, 1, 1),
      mShaderEffect(ct_sharedSolidShaderEffect())
{

}

CtRectSprite::CtRectSprite(ctreal r, ctreal g, ctreal b, CtSprite *parent)
    : CtSprite(parent),
      mColor(r, g, b),
      mShaderEffect(ct_sharedSolidShaderEffect())
{

}

CtRectSprite::~CtRectSprite()
{

}

void CtRectSprite::paint(CtRenderer *renderer)
{
    renderer->drawSolid(mShaderEffect, width(), height(),
                        mColor.red(), mColor.green(), mColor.blue(), mColor.alpha());
}

void CtRectSprite::setColor(const CtColor &color)
{
    mColor = color;
}

void CtRectSprite::setShaderEffect(CtShaderEffect *effect)
{
    mShaderEffect = effect;
}

/////////////////////////////////////////////////
// CtTextSprite
/////////////////////////////////////////////////

void CtTextSprite::releaseBuffers()
{
    if (mIndexBuffer > 0) {
        CtGL::glDeleteBuffers(1, &mIndexBuffer);
        mIndexBuffer = 0;
    }

    if (mVertexBuffer > 0) {
        CtGL::glDeleteBuffers(1, &mVertexBuffer);
        mVertexBuffer = 0;
    }

    mGlyphCount = 0;
}

void CtTextSprite::recreateBuffers()
{
    releaseBuffers();

    const char *str = mText.data();
    const int len = strlen(str);

    if (!mFont || len == 0)
        return;

    GLfloat vertices[24 * len];
    unsigned short indexes[6 * len];

    int n = 0;
    int j = 0;
    int fx = 0;

    for (int i = 0; i < len; i++) {
        CtFontGlyph *glyph = mFont->mGlyphs.value((wchar_t)str[i], 0);

        if (!glyph)
            continue;

        const int offset = n * 24;
        const int x  = glyph->xOffset;
        const int y  = glyph->yOffset;
        const int w  = glyph->width;
        const int h  = glyph->height;
        const int by = mFont->mAscender;

        ct_setTriangle2Array(vertices + offset,
                             fx + x, by - y, fx + x + w, by - y + h,
                             glyph->s0, glyph->t0, glyph->s1, glyph->t1);

        fx += glyph->xAdvance;

        // indexes
        for (int kk = j + 6; j < kk; j++)
            indexes[j] = j;

        n++;
    }

    if (n > 0) {
        CtGL::glGenBuffers(1, &mVertexBuffer);
        CtGL::glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        CtGL::glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        CtGL::glGenBuffers(1, &mIndexBuffer);
        CtGL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
        CtGL::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

        CtGL::glBindBuffer(GL_ARRAY_BUFFER, 0);
        CtGL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    mGlyphCount = n;
}

CtTextSprite::CtTextSprite(CtSprite *parent)
    : CtSprite(parent),
      mColor(0, 0, 0),
      mText(),
      mGlyphCount(0),
      mIndexBuffer(0),
      mVertexBuffer(0),
      mFont(0),
      mShaderEffect(ct_sharedTextShaderEffect())
{

}

CtTextSprite::~CtTextSprite()
{
    releaseBuffers();
}

void CtTextSprite::paint(CtRenderer *renderer)
{
    if (!mFont || !mShaderEffect || mGlyphCount <= 0)
        return;

    if (!mFont->mAtlas)
        return;

    renderer->drawVboTextTexture(mShaderEffect, mFont->mAtlas, mIndexBuffer,
                                 mVertexBuffer, mGlyphCount, mColor);
}

void CtTextSprite::setColor(const CtColor &color)
{
    mColor = color;
}

void CtTextSprite::setText(const CtString &text)
{
    if (mText != text) {
        mText = text;
        recreateBuffers();
    }
}

void CtTextSprite::setFont(CtFont *font)
{
    if (mFont != font) {
        mFont = font;
        recreateBuffers();
    }
}

void CtTextSprite::setShaderEffect(CtShaderEffect *effect)
{
    mShaderEffect = effect;
}

/////////////////////////////////////////////////
// CtFrameBufferSprite
/////////////////////////////////////////////////

void CtFrameBufferSprite::recursivePaint(CtRenderer *renderer)
{
    if (!mTexture->isValid()) {
        // invalid framebuffer
        CtSprite::recursivePaint(renderer);
        return;
    }

    renderer->bindBuffer(mFramebuffer);

    const CtList<CtSprite *> &lst = orderedChildren();

    foreach (CtSprite *item, lst) {
        if (item->isVisible())
            item->recursivePaint(renderer);
    }

    renderer->releaseBuffer();

    renderer->begin();
    renderer->m_opacity = relativeOpacity();
    renderer->m_projectionMatrix = currentViewportProjectionMatrix();
    paint(renderer);
    renderer->end();
}

void CtFrameBufferSprite::deleteBuffers()
{
    if (mFramebuffer > 0) {
        CtGL::glDeleteFramebuffers(1, &mFramebuffer);
        mFramebuffer = 0;
    }

    if (mColorbuffer > 0) {
        CtGL::glDeleteRenderbuffers(1, &mColorbuffer);
        mColorbuffer = 0;
    }
}

void CtFrameBufferSprite::resizeBuffer(int w, int h)
{
    if (w == mBufferWidth && h == mBufferHeight)
        return;

    mBufferWidth = w;
    mBufferHeight = h;

    deleteBuffers();
    mTexture->release();

    if (w <= 0 || h <= 0)
        return;

    mTexture->loadFromData(w, h, 4, 0);

    GLint defaultFBO;
    CtGL::glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);

    CtGL::glGenFramebuffers(1, &mFramebuffer);
    CtGL::glGenRenderbuffers(1, &mColorbuffer);

    // bind
    CtGL::glBindFramebuffer(GL_FRAMEBUFFER, mFramebuffer);
    CtGL::glBindRenderbuffer(GL_RENDERBUFFER, mColorbuffer);

    CtGL::glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, w, h);
    CtGL::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                    GL_RENDERBUFFER, mColorbuffer);

    CtGL::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                 GL_TEXTURE_2D, mTexture->id(), 0);

    CtGL::glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &mBufferWidth);
    CtGL::glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &mBufferHeight);

    // check if framebuffer is ready
    GLuint status = CtGL::glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
        CT_WARNING("Could not create framebuffer");

    CtGL::glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
}


CtFrameBufferSprite::CtFrameBufferSprite(CtSprite *parent)
    : CtSprite(parent),
      mBufferWidth(0),
      mBufferHeight(0),
      mFramebuffer(0),
      mColorbuffer(0),
      mTexture(new CtTexture()),
      mShaderEffect(ct_sharedTextureShaderEffect())
{
    setIsFrameBuffer(true);
}

CtFrameBufferSprite::~CtFrameBufferSprite()
{
    deleteBuffers();
    delete mTexture;
}

void CtFrameBufferSprite::paint(CtRenderer *renderer)
{
    renderer->drawTexture(mShaderEffect, mTexture, width(), height());
}

void CtFrameBufferSprite::setShaderEffect(CtShaderEffect *effect)
{
    mShaderEffect = effect;
}

bool CtFrameBufferSprite::isValidBuffer() const
{
    return mTexture->isValid();
}

void CtFrameBufferSprite::setBufferSize(int width, int height)
{
    resizeBuffer(width, height);
}

/////////////////////////////////////////////////
// CtTextureSprite
/////////////////////////////////////////////////

CtTextureSprite::CtTextureSprite(CtSprite *parent)
    : CtSprite(parent),
      mTextureAtlasIndex(-1),
      mOwnTexture(false),
      mTexture(0),
      mShaderEffect(ct_sharedTextureShaderEffect())
{

}

CtTextureSprite::CtTextureSprite(CtTexture *texture, CtSprite *parent)
    : CtSprite(parent),
      mTextureAtlasIndex(-1),
      mOwnTexture(false),
      mTexture(texture),
      mShaderEffect(ct_sharedTextureShaderEffect())
{
    if (texture) {
        setWidth(texture->width());
        setHeight(texture->height());
    }
}

CtTextureSprite::~CtTextureSprite()
{
    releaseTexture();
}

void CtTextureSprite::releaseTexture()
{
    if (mTexture && mOwnTexture) {
        delete mTexture;
        mTexture = 0;
        mOwnTexture = false;
    }
}

void CtTextureSprite::setTexture(CtTexture *texture)
{
    if (mTexture == texture)
        return;

    releaseTexture();
    mTexture = texture;
}

void CtTextureSprite::setTextureAtlasIndex(int index)
{
    mTextureAtlasIndex = index;
}

void CtTextureSprite::setShaderEffect(CtShaderEffect *effect)
{
    mShaderEffect = effect;
}

bool CtTextureSprite::load(const CtString &filePath)
{
    if (!mOwnTexture) {
        mTexture = new CtTexture();
        mOwnTexture = true;
    }

    if (!mTexture->load(filePath))
        return false;

    setWidth(mTexture->width());
    setHeight(mTexture->height());

    return true;
}

/////////////////////////////////////////////////
// CtImageSprite
/////////////////////////////////////////////////

CtImageSprite::CtImageSprite(CtSprite *parent)
    : CtTextureSprite(parent),
      mFillMode(CtImageSprite::Stretch)
{

}

CtImageSprite::CtImageSprite(CtTexture *texture, CtSprite *parent)
    : CtTextureSprite(texture, parent),
      mFillMode(CtImageSprite::Stretch)
{

}

CtImageSprite::~CtImageSprite()
{

}

void CtImageSprite::paint(CtRenderer *renderer)
{
    const bool vTile = (mFillMode == CtImageSprite::Tile ||
                        mFillMode == CtImageSprite::TileVertically);

    const bool hTile = (mFillMode == CtImageSprite::Tile ||
                        mFillMode == CtImageSprite::TileHorizontally);

    renderer->drawTexture(shaderEffect(), texture(), width(), height(),
                          vTile, hTile, textureAtlasIndex());
}

/////////////////////////////////////////////////
// CtImagePolygonSprite
/////////////////////////////////////////////////

CtImagePolygonSprite::CtImagePolygonSprite(CtSprite *parent)
    : CtImageSprite(parent),
      mVertices()
{

}

CtImagePolygonSprite::CtImagePolygonSprite(CtTexture *texture, CtSprite *parent)
    : CtImageSprite(texture, parent),
      mVertices()
{

}

CtImagePolygonSprite::~CtImagePolygonSprite()
{

}

void CtImagePolygonSprite::paint(CtRenderer *renderer)
{
    const int count = mVertices.size();

    if (!texture() || count < 3)
        return;

    if (textureAtlasIndex() >= 0) {
        CT_WARNING("CtImagePolygonSprite does not support atlas texture yet!");
        return;
    }

    FillMode mode = fillMode();

    const bool vTile = (mode == CtImagePolygonSprite::Tile ||
                        mode == CtImagePolygonSprite::TileVertically);

    const bool hTile = (mode == CtImagePolygonSprite::Tile ||
                        mode == CtImagePolygonSprite::TileHorizontally);

    int i = 0;
    GLfloat *vertices = new GLfloat[count * 2];
    GLfloat *texCoords = new GLfloat[count * 2];

    const ctreal w = hTile ? texture()->width() : width();
    const ctreal h = vTile ? texture()->height() : height();

    foreach (const CtPoint &p, mVertices) {
        vertices[i] = p.x();
        vertices[i + 1] = p.y();

        texCoords[i] = p.x() / w;
        texCoords[i + 1] = p.y() / h;

        i += 2;
    }

    renderer->drawTexture(shaderEffect(), texture(), vertices, texCoords, count,
                          vTile, hTile);

    delete [] vertices;
    delete [] texCoords;
}


/////////////////////////////////////////////////
// CtFragmentsSprite
/////////////////////////////////////////////////

CtFragmentsSprite::Fragment::Fragment()
    : mX(0),
      mY(0),
      mWidth(0),
      mHeight(0),
      mOpacity(1.0),
      mAtlasIndex(-1),
      mUserData(0)
{

}


CtFragmentsSprite::CtFragmentsSprite(CtSprite *parent)
    : CtTextureSprite(parent),
      mFragments()
{
    setShaderEffect(ct_sharedFragmentShaderEffect());
}

CtFragmentsSprite::CtFragmentsSprite(CtTexture *texture, CtSprite *parent)
    : CtTextureSprite(texture, parent),
      mFragments()
{
    setShaderEffect(ct_sharedFragmentShaderEffect());
}

CtFragmentsSprite::~CtFragmentsSprite()
{
    clearFragments();
}

void CtFragmentsSprite::clearFragments()
{
    foreach (Fragment *f, mFragments)
        delete f;

    mFragments.clear();
}

bool CtFragmentsSprite::appendFragment(Fragment *fragment)
{
    return insertFragment(-1, fragment);
}

bool CtFragmentsSprite::insertFragment(int index, Fragment *fragment)
{
    foreach (Fragment *f, mFragments) {
        if (fragment == f)
            return false;
    }

    if (index < 0 || index >= (int)mFragments.size()) {
        mFragments.append(fragment);
    } else {
        CtList<Fragment *>::iterator it;
        it = mFragments.begin();
        std::advance(it, index);
        mFragments.insert(it, fragment);
    }

    return true;
}

bool CtFragmentsSprite::removeFragment(Fragment *fragment)
{
    CtList<Fragment *>::iterator it;

    for (it = mFragments.begin(); it != mFragments.end(); it++) {
        if (*it == fragment) {
            mFragments.remove(it);
            delete fragment;
            return true;
        }
    }

    return false;
}

void CtFragmentsSprite::paint(CtRenderer *renderer)
{
    if (!shaderEffect())
        return;

    CtShaderEffect::Element e;
    CtList<CtShaderEffect::Element> elements;

    foreach (CtFragmentsSprite::Fragment *f, mFragments) {
        e.x = f->x();
        e.y = f->y();
        e.width = f->width();
        e.height = f->height();
        e.opacity = f->opacity();
        e.textureAtlasIndex = f->atlasIndex();
        elements.append(e);
    }

    renderer->drawElements(shaderEffect(), texture(), elements);
}


/////////////////////////////////////////////////
// CtParticlesSprite
/////////////////////////////////////////////////

CtParticlesSprite::Particle::Particle()
    : mX(0),
      mY(0),
      mSize(10),
      mColor(1, 1, 1, 1),
      mUserData(0)
{

}


CtParticlesSprite::CtParticlesSprite(CtSprite *parent)
    : CtTextureSprite(parent),
      mVertices(0),
      mAttrCount(7),
      mVertexSize(7 * sizeof(GLfloat)),
      mVertexCount(0),
      mParticles()
{
    setShaderEffect(ct_sharedParticleShaderEffect());
}

CtParticlesSprite::CtParticlesSprite(CtTexture *texture, CtSprite *parent)
    : CtTextureSprite(texture, parent),
      mVertices(0),
      mAttrCount(7),
      mVertexSize(7 * sizeof(GLfloat)),
      mVertexCount(0),
      mParticles()
{
    setShaderEffect(ct_sharedParticleShaderEffect());
}

CtParticlesSprite::~CtParticlesSprite()
{
    foreach (Particle *p, mParticles)
        delete p;

    mParticles.clear();

    if (mVertices) {
        delete [] mVertices;
        mVertices = 0;
    }
}

void CtParticlesSprite::recreateVertexBuffer()
{
    if (mVertices) {
        delete [] mVertices;
        mVertices = 0;
    }

    mVertexCount = mParticles.size();
    const size_t size = mVertexSize * mVertexCount;

    if (size > 0)
        mVertices = new GLfloat[size];
}

bool CtParticlesSprite::addParticle(Particle *particle)
{
    foreach (Particle *p, mParticles) {
        if (particle == p)
            return false;
    }

    mParticles.append(particle);
    recreateVertexBuffer();
    return true;
}

bool CtParticlesSprite::removeParticle(Particle *fragment)
{
    bool found = false;
    CtVector<Particle *>::iterator it;

    for (it = mParticles.begin(); it != mParticles.end(); it++) {
        if (*it == fragment) {
            mParticles.remove(it);
            delete fragment;

            found = true;
            break;
        }
    }

    if (found)
        recreateVertexBuffer();

    return found;
}

void CtParticlesSprite::clearParticles()
{
    foreach (Particle *p, mParticles)
        delete p;

    mParticles.clear();
    recreateVertexBuffer();
}

void CtParticlesSprite::paint(CtRenderer *renderer)
{
    CtTexture *tex = texture();

    if (!tex || !mVertices || !shaderEffect() || !shaderEffect()->init())
        return;

    GLfloat *vptr = mVertices;

    foreach (CtParticlesSprite::Particle *p, mParticles) {
        const CtColor &c = p->color();

        vptr[0] = p->x();
        vptr[1] = p->y();
        vptr[2] = p->pointSize();
        vptr[3] = c.red();
        vptr[4] = c.green();
        vptr[5] = c.blue();
        vptr[6] = c.alpha();

        vptr += mAttrCount;
    }

    // use program
    CtGpuProgram *program = shaderEffect()->program();

    int locMatrix = program->uniformLocation("ct_Matrix");
    int locOpacity = program->uniformLocation("ct_Opacity");
    int locPosition = program->attributeLocation("ct_Vertex");
    int locTexture = program->uniformLocation("ct_Texture0");
    int locColor = program->attributeLocation("a_color");
    int locPointSize = program->attributeLocation("a_size");

    program->bind();

    // set matrix and relative opacity
    program->setUniformValue(locMatrix, renderer->projectionMatrix());
    program->setUniformValue(locOpacity, renderer->opacity());

    // enable attributes
    program->enableVertexAttributeArray(locPosition);
    program->enableVertexAttributeArray(locPointSize);
    program->enableVertexAttributeArray(locColor);

    GLfloat *vertexPtr = mVertices;
    program->setVertexAttributePointer(locPosition, GL_FLOAT, 2, mVertexSize, vertexPtr);
    vertexPtr += 2;
    program->setVertexAttributePointer(locPointSize, GL_FLOAT, 1, mVertexSize, vertexPtr);
    vertexPtr += 1;
    program->setVertexAttributePointer(locColor, GL_FLOAT, 4, mVertexSize, vertexPtr);

    // apply texture
    CtGL::glActiveTexture(GL_TEXTURE0);
    CtGL::glBindTexture(GL_TEXTURE_2D, tex->id());
    program->setUniformValue(locTexture, int(0));
    CtGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    CtGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    CtGL::glDrawArrays(GL_POINTS, 0, mVertexCount);

    program->release();
}
