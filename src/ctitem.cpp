#include "ctitem.h"
#include "ctmath.h"
#include "ctitem_p.h"
#include "ctrenderer.h"
#include "ctsceneview.h"
#include "ctdragcursor.h"
#include "ctshadereffect.h"
#include "3rdparty/tricollision.h"
#include "freetype-gl.h"


static CtShaderEffect *ct_sharedSolidShaderEffect()
{
    static CtShaderEffect *r =
        new CtShaderEffect(CtShaderProgram::sharedSolidShaderProgram());

    return r;
}

static CtShaderEffect *ct_sharedTextureShaderEffect()
{
    static CtShaderEffect *r =
        new CtShaderEffect(CtShaderProgram::sharedTextureShaderProgram());
    return r;
}

static CtShaderEffect *ct_sharedTextShaderEffect()
{
    static CtShaderEffect *r
        = new CtShaderEffect(CtShaderProgram::sharedTextShaderProgram());
    return r;
}

static CtShaderEffect *ct_sharedFragmentShaderEffect()
{
    static CtShaderEffect *r
        = new CtShaderEffect(CtShaderProgram::sharedFragmentShaderProgram());
    return r;
}

static CtShaderEffect *ct_sharedParticleShaderEffect()
{
    static CtShaderEffect *r
        = new CtShaderEffect(CtShaderProgram::sharedParticleShaderProgram());
    return r;
}

bool CtSpritePrivate_sort_compare(CtSprite *a, CtSprite *b)
{
    return (!a || !b) ? false : (a->z() < b->z());
}


CtSpritePrivate::CtSpritePrivate(CtSprite *q)
    : q(q),
      x(0),
      y(0),
      z(0),
      width(0),
      height(0),
      xScale(1),
      yScale(1),
      rotation(0),
      opacity(1.0),
      visible(true),
      isFrozen(false),
      implicitWidth(0),
      implicitHeight(0),
      parent(0),
      scene(0),
      xCenter(0),
      yCenter(0),
      sortDirty(false),
      transformDirty(true),
      flags(CtSprite::NoFlag),
      isFrameBuffer(false),
      pendingDelete(false)
{

}

CtSpritePrivate::~CtSpritePrivate()
{

}

void CtSpritePrivate::init(CtSprite *p)
{
    parent = p;
    if (parent)
        parent->d_ptr->addItem(q);
}

void CtSpritePrivate::release()
{
    if (parent)
        parent->d_ptr->removeItem(q);

    // delete children
    CtList<CtSprite *> oldChildren = children;

    foreach (CtSprite *item, oldChildren)
        delete item;
}

ctreal CtSpritePrivate::relativeOpacity()
{
    // XXX: optimize
    if (!parent || opacity == 0.0 || (flags & CtSprite::IgnoreAllParentOpacity)) {
        return opacity;
    } else if ((flags & CtSprite::IgnoreParentOpacity)) {
        CtSprite *topParent = parent ? parent->parent() : 0;

        if (!topParent)
            return opacity;
        else
            return opacity * topParent->opacity();
    } else {
        return opacity * parent->opacity();
    }
}

bool CtSpritePrivate::relativeVisible()
{
    // XXX: optimize
    if (!parent)
        return visible;
    else
        return visible && parent->isVisible();
}

bool CtSpritePrivate::relativeFrozen()
{
    // XXX: optimize
    if (!parent)
        return isFrozen;
    else
        return isFrozen || parent->isFrozen();
}

CtSceneFrameBuffer *CtSpritePrivate::frameBufferItem()
{
    // XXX: optimize
    if (!parent)
        return 0;
    else if (parent->d_ptr->isFrameBuffer)
        return static_cast<CtSceneFrameBuffer *>(parent);
    else
        return parent->d_ptr->frameBufferItem();
}

void CtSpritePrivate::fillItems(CtList<CtSprite *> &lst)
{
    lst.append(q);

    const CtList<CtSprite *> &items = orderedChildren();

    foreach (CtSprite *item, items)
        item->d_ptr->fillItems(lst);
}

const CtList<CtSprite *> &CtSpritePrivate::orderedChildren()
{
    if (!sortDirty)
        return sortedChildren;

    sortDirty = false;
    sortedChildren = children;
    sortedChildren.sort(CtSpritePrivate_sort_compare);

    return sortedChildren;
}

void CtSpritePrivate::recursivePaint(CtRenderer *renderer)
{
    if ((flags & CtSprite::DoNotPaintContent) == 0) {
        renderer->begin();
        renderer->m_opacity = relativeOpacity();
        renderer->m_projectionMatrix = currentViewportProjectionMatrix();
        q->paint(renderer);
        renderer->end();
    }

    const CtList<CtSprite *> &lst = orderedChildren();

    foreach (CtSprite *item, lst) {
        if (item->isVisible())
            item->d_ptr->recursivePaint(renderer);
    }
}

void CtSpritePrivate::addItem(CtSprite *item)
{
    if (children.contains(item))
        return;

    children.removeAll(item);
    children.append(item);

    sortDirty = true;

    CtSceneView *mScene = q->scene();

    if (mScene)
        mScene->itemAddedToScene(q);

    CtSprite::ChangeValue value;
    value.itemValue = item;
    q->itemChanged(CtSprite::ChildAdded, value);
}

void CtSpritePrivate::removeItem(CtSprite *item)
{
    if (!children.contains(item))
        return;

    children.removeAll(item);

    sortDirty = true;

    CtSceneView *mScene = q->scene();

    if (mScene)
        mScene->itemRemovedFromScene(q);

    CtSprite::ChangeValue value;
    value.itemValue = item;
    q->itemChanged(CtSprite::ChildRemoved, value);
}

void CtSpritePrivate::setScene(CtSceneView *newScene)
{
    scene = newScene;
    // update cache
}

CtMatrix CtSpritePrivate::mappedTransformMatrix(CtSprite *root)
{
    const ctreal ox = ctRound(xCenter);
    const ctreal oy = ctRound(yCenter);

    // pixel aligned
    const ctreal rx = ctRound(x);
    const ctreal ry = ctRound(y);

    CtMatrix modelMatrix;
    modelMatrix.translate(rx, ry);

    modelMatrix.translate(ox, oy);
    modelMatrix.scale(xScale, yScale);
    modelMatrix.rotate(-(GLfloat)rotation);
    modelMatrix.translate(-ox, -oy);

    CtMatrix m = localMatrix;
    m.multiply(modelMatrix);
    modelMatrix.setMatrix(m);

    if (parent && (parent != root))
        modelMatrix.multiply(parent->d_ptr->mappedTransformMatrix(root));

    if (!root) {
        sceneTransformMatrix = modelMatrix;
        localTransformMatrix = modelMatrix;
        localTransformMatrix.invert();
    }

    return modelMatrix;
}

void CtSpritePrivate::checkTransformMatrix()
{
    if (!transformDirty)
        return;

    sceneTransformMatrix = mappedTransformMatrix(0);
    localTransformMatrix = sceneTransformMatrix;
    localTransformMatrix.invert();

    // fbo
    CtSceneFrameBuffer *fbo = frameBufferItem();

    if (!fbo || !fbo->isValidBuffer())
        fboTransformMatrix = sceneTransformMatrix;
    else
        fboTransformMatrix = mappedTransformMatrix(fbo);

    //transformDirty = true; // XXX: enable for optimization
}

CtMatrix CtSpritePrivate::currentSceneTransformMatrix()
{
    checkTransformMatrix();
    return sceneTransformMatrix;
}

CtMatrix CtSpritePrivate::currentLocalTransformMatrix()
{
    checkTransformMatrix();
    return localTransformMatrix;
}

CtMatrix4x4 CtSpritePrivate::currentViewportProjectionMatrix()
{
    CtSceneView *sc = q->scene();
    CtSceneFrameBuffer *fb = frameBufferItem();

    checkTransformMatrix();

    CtMatrix4x4 result;

    if (sc) {
        if (!fb || !fb->isValidBuffer())
            result = sceneTransformMatrix.toMatrix4x4();
        else
            result = fboTransformMatrix.toMatrix4x4();

        CtMatrix4x4 ortho;
        ortho.ortho(0, sc->width(), sc->height(), 0, 1, -1);
        result.multiply(ortho);
    }

    return result;
}

CtSprite::CtSprite(CtSprite *parent)
    : CtObject(parent),
      d_ptr(new CtSpritePrivate(this))
{
    CT_D(CtSprite);
    d->init(parent);
}

CtSprite::CtSprite(CtSpritePrivate *dd)
    : d_ptr(dd)
{

}

CtSprite::~CtSprite()
{
    CT_D(CtSprite);
    d->release();
    delete d;
}

CtSceneView *CtSprite::scene() const
{
    CT_D(CtSprite);
    if (d->parent)
        return d->parent->scene();
    else
        return d->scene;
}

CtSprite *CtSprite::parent() const
{
    CT_D(CtSprite);
    return d->parent;
}

ctreal CtSprite::x() const
{
    CT_D(CtSprite);
    return d->x;
}

void CtSprite::setX(ctreal x)
{
    CT_D(CtSprite);
    if (d->x == x)
        return;

    d->x = x;

    ChangeValue value;
    value.realValue = x;
    itemChanged(XChange, value);
}

ctreal CtSprite::y() const
{
    CT_D(CtSprite);
    return d->y;
}

void CtSprite::setY(ctreal y)
{
    CT_D(CtSprite);
    if (d->y == y)
        return;

    d->y = y;

    ChangeValue value;
    value.realValue = y;
    itemChanged(YChange, value);
}

ctreal CtSprite::z() const
{
    CT_D(CtSprite);
    return d->z;
}

void CtSprite::setZ(ctreal z)
{
    CT_D(CtSprite);
    if (d->z == z)
        return;

    d->z = z;

    ChangeValue value;
    value.realValue = z;
    itemChanged(ZChange, value);
}

ctreal CtSprite::rotation() const
{
    CT_D(CtSprite);
    return d->rotation;
}

void CtSprite::setRotation(ctreal rotation)
{
    CT_D(CtSprite);
    if (d->rotation == rotation)
        return;

    d->rotation = rotation;

    ChangeValue value;
    value.realValue = rotation;
    itemChanged(RotationChange, value);
}

bool CtSprite::isFrozen() const
{
    CT_D(CtSprite);
    return d->relativeFrozen();
}

void CtSprite::setFrozen(bool frozen)
{
    CT_D(CtSprite);
    if (d->isFrozen == frozen)
        return;

    d->isFrozen = frozen;

    // XXX: should emit when parent changes too
    ChangeValue value;
    value.boolValue = frozen;
    itemChanged(FrozenChange, value);
}

ctreal CtSprite::xScale() const
{
    CT_D(CtSprite);
    return d->xScale;
}

void CtSprite::setXScale(ctreal scale)
{
    CT_D(CtSprite);
    if (d->xScale == scale)
        return;

    d->xScale = scale;

    ChangeValue value;
    value.realValue = scale;
    itemChanged(XScaleChange, value);
}

ctreal CtSprite::yScale() const
{
    CT_D(CtSprite);
    return d->yScale;
}

void CtSprite::setYScale(ctreal scale)
{
    CT_D(CtSprite);
    if (d->yScale == scale)
        return;

    d->yScale = scale;

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
    CT_D(CtSprite);
    return d->relativeOpacity();
}

void CtSprite::setOpacity(ctreal opacity)
{
    CT_D(CtSprite);
    opacity = ctClamp<ctreal>(0, opacity, 1);

    if (d->opacity == opacity)
        return;

    d->opacity = opacity;

    ChangeValue value;
    value.realValue = opacity;
    itemChanged(OpacityChange, value);
}

int CtSprite::flags() const
{
    CT_D(CtSprite);
    return d->flags;
}

void CtSprite::setFlag(Flag flag, bool enabled)
{
    CT_D(CtSprite);
    if (enabled) {
        d->flags |= flag;
    } else {
        if ((d->flags & flag))
            d->flags ^= flag;
    }
}

void CtSprite::deleteLater()
{
    CT_D(CtSprite);
    d->pendingDelete = true;
}

ctreal CtSprite::width() const
{
    CT_D(CtSprite);
    return d->width;
}

void CtSprite::setWidth(ctreal width)
{
    CT_D(CtSprite);
    if (d->width == width)
        return;

    d->width = width;

    ChangeValue value;
    value.realValue = width;
    itemChanged(WidthChange, value);
}

ctreal CtSprite::height() const
{
    CT_D(CtSprite);
    return d->height;
}

void CtSprite::setHeight(ctreal height)
{
    CT_D(CtSprite);
    if (d->height == height)
        return;

    d->height = height;

    ChangeValue value;
    value.realValue = height;
    itemChanged(HeightChange, value);
}

ctreal CtSprite::implicitWidth() const
{
    CT_D(CtSprite);
    return d->implicitWidth;
}

void CtSprite::setImplicitWidth(ctreal width)
{
    CT_D(CtSprite);
    if (d->implicitWidth == width)
        return;

    d->implicitWidth = width;

    ChangeValue value;
    value.realValue = width;
    itemChanged(ImplicitWidthChange, value);
}

ctreal CtSprite::implicitHeight() const
{
    CT_D(CtSprite);
    return d->implicitHeight;
}

void CtSprite::setImplicitHeight(ctreal height)
{
    CT_D(CtSprite);
    if (d->implicitHeight == height)
        return;

    d->implicitHeight = height;

    ChangeValue value;
    value.realValue = height;
    itemChanged(ImplicitHeightChange, value);
}

void CtSprite::resize(ctreal width, ctreal height)
{
    setWidth(width);
    setHeight(height);
}

bool CtSprite::contains(ctreal x, ctreal y)
{
    CT_D(CtSprite);
    return (x >= 0 && y >= 0 && x < d->width && y < d->height);
}

void CtSprite::paint(CtRenderer *renderer)
{
    CT_UNUSED(renderer);
}

void CtSprite::advance(ctuint ms)
{
    CT_UNUSED(ms);
}

bool CtSprite::isVisible() const
{
    CT_D(CtSprite);
    return d->relativeVisible();
}

void CtSprite::setVisible(bool visible)
{
    CT_D(CtSprite);
    if (d->visible == visible)
        return;

    d->visible = visible;

    // XXX: should emit when parent changes too
    ChangeValue value;
    value.boolValue = visible;
    itemChanged(VisibilityChange, value);
}

void CtSprite::translate(ctreal x, ctreal y)
{
    CT_D(CtSprite);
    d->localMatrix.translate(x, y);
}

CtRect CtSprite::boundingRect() const
{
    CT_D(CtSprite);
    return CtRect(0, 0, d->width, d->height);
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
    CT_D(CtSprite);
    return CtPoint(d->xCenter, d->yCenter);
}

void CtSprite::setTransformOrigin(ctreal x, ctreal y)
{
    CT_D(CtSprite);
    d->xCenter = x;
    d->yCenter = y;
}

CtList<CtSprite *> CtSprite::children() const
{
    CT_D(CtSprite);
    return d->children;
}

CtMatrix CtSprite::transformMatrix() const
{
    CT_D(CtSprite);
    return d->currentLocalTransformMatrix();
}

CtMatrix CtSprite::sceneTransformMatrix() const
{
    CT_D(CtSprite);
    return d->currentSceneTransformMatrix();
}

void CtSprite::setLocalTransform(const CtMatrix &matrix)
{
    CT_D(CtSprite);
    d->localMatrix = matrix;
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
// CtSceneRect
/////////////////////////////////////////////////

CtSceneRectPrivate::CtSceneRectPrivate(CtSceneRect *q)
    : CtSpritePrivate(q),
      shaderEffect(0)
{

}

void CtSceneRectPrivate::init(CtSprite *parent)
{
    CtSpritePrivate::init(parent);
    shaderEffect = ct_sharedSolidShaderEffect();
}

void CtSceneRectPrivate::release()
{
    CtSpritePrivate::release();
}


CtSceneRect::CtSceneRect(CtSprite *parent)
    : CtSprite(new CtSceneRectPrivate(this))
{
    CT_D(CtSceneRect);
    d->init(parent);
}

CtSceneRect::CtSceneRect(ctreal r, ctreal g, ctreal b, CtSprite *parent)
    : CtSprite(new CtSceneRectPrivate(this))
{
    CT_D(CtSceneRect);
    d->init(parent);
    d->color = CtColor(r, g, b);
}

void CtSceneRect::paint(CtRenderer *renderer)
{
    CT_D(CtSceneRect);
    renderer->drawSolid(d->shaderEffect, d->width, d->height,
                        d->color.red(), d->color.green(), d->color.blue(), d->color.alpha());
}

CtColor CtSceneRect::color() const
{
    CT_D(CtSceneRect);
    return d->color;
}

void CtSceneRect::setColor(const CtColor &color)
{
    CT_D(CtSceneRect);
    d->color = color;
}

CtShaderEffect *CtSceneRect::shaderEffect() const
{
    CT_D(CtSceneRect);
    return d->shaderEffect;
}

void CtSceneRect::setShaderEffect(CtShaderEffect *effect)
{
    CT_D(CtSceneRect);
    d->shaderEffect = effect;
}

/////////////////////////////////////////////////
// CtSceneText
/////////////////////////////////////////////////

CtSceneTextPrivate::CtSceneTextPrivate(CtSceneText *q)
    : CtSpritePrivate(q),
      color(0, 0, 0),
      glyphCount(0),
      indexBuffer(0),
      vertexBuffer(0),
      font(0),
      shaderEffect(0)
{

}

void CtSceneTextPrivate::init(CtSprite *parent)
{
    CtSpritePrivate::init(parent);
    shaderEffect = ct_sharedTextShaderEffect();
}

void CtSceneTextPrivate::release()
{
    CtSpritePrivate::release();
    releaseBuffers();
}

void CtSceneTextPrivate::releaseBuffers()
{
    if (indexBuffer > 0) {
        glDeleteBuffers(1, &indexBuffer);
        indexBuffer = 0;
    }

    if (vertexBuffer > 0) {
        glDeleteBuffers(1, &vertexBuffer);
        vertexBuffer = 0;
    }

    glyphCount = 0;
}

void CtSceneTextPrivate::recreateBuffers()
{
    releaseBuffers();

    const char *str = text.data();
    const int len = strlen(str);

    if (!font || len == 0)
        return;

    GLfloat vertices[24 * len];
    unsigned short indexes[6 * len];

    int n = 0;
    int j = 0;
    int fx = 0;

    for (int i = 0; i < len; i++) {
        CtFontGlyph *glyph = font->mGlyphs.value((wchar_t)str[i], 0);

        if (!glyph)
            continue;

        const int offset = n * 24;
        const int x  = glyph->xOffset;
        const int y  = glyph->yOffset;
        const int w  = glyph->width;
        const int h  = glyph->height;
        const int by = font->mAscender;

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
        CtGL::glGenBuffers(1, &vertexBuffer);
        CtGL::glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        CtGL::glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        CtGL::glGenBuffers(1, &indexBuffer);
        CtGL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        CtGL::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

        CtGL::glBindBuffer(GL_ARRAY_BUFFER, 0);
        CtGL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    glyphCount = n;
}

CtSceneText::CtSceneText(CtSprite *parent)
    : CtSprite(new CtSceneTextPrivate(this))
{
    CT_D(CtSceneText);
    d->init(parent);
}

void CtSceneText::paint(CtRenderer *renderer)
{
    CT_D(CtSceneText);

    if (!d->font || !d->shaderEffect || d->glyphCount <= 0)
        return;

    if (!d->font->mAtlas)
        return;

    renderer->drawVboTextTexture(d->shaderEffect, d->font->mAtlas, d->indexBuffer,
                                 d->vertexBuffer, d->glyphCount, d->color);
}

CtColor CtSceneText::color() const
{
    CT_D(CtSceneText);
    return d->color;
}

void CtSceneText::setColor(const CtColor &color)
{
    CT_D(CtSceneText);
    d->color = color;
}

CtString CtSceneText::text() const
{
    CT_D(CtSceneText);
    return d->text;
}

void CtSceneText::setText(const CtString &text)
{
    CT_D(CtSceneText);
    if (d->text != text) {
        d->text = text;
        d->recreateBuffers();
    }
}

CtFont *CtSceneText::font() const
{
    CT_D(CtSceneText);
    return d->font;
}

void CtSceneText::setFont(CtFont *font)
{
    CT_D(CtSceneText);
    if (d->font != font) {
        d->font = font;
        d->recreateBuffers();
    }
}

CtShaderEffect *CtSceneText::shaderEffect() const
{
    CT_D(CtSceneText);
    return d->shaderEffect;
}

void CtSceneText::setShaderEffect(CtShaderEffect *effect)
{
    CT_D(CtSceneText);
    d->shaderEffect = effect;
}

/////////////////////////////////////////////////
// CtSceneFrameBuffer
/////////////////////////////////////////////////

CtSceneFrameBufferPrivate::CtSceneFrameBufferPrivate(CtSceneFrameBuffer *q)
    : CtSpritePrivate(q),
      bufferWidth(0),
      bufferHeight(0),
      framebuffer(0),
      depthbuffer(0),
      texture(0),
      shaderEffect(0)
{
    isFrameBuffer = true;
}

void CtSceneFrameBufferPrivate::init(CtSprite *parent)
{
    CtSpritePrivate::init(parent);

    shaderEffect = ct_sharedTextureShaderEffect();
    texture = new CtTexture();
}

void CtSceneFrameBufferPrivate::release()
{
    CtSpritePrivate::release();
    delete texture;
}

void CtSceneFrameBufferPrivate::recursivePaint(CtRenderer *renderer)
{
    if (!texture->isValid()) {
        // invalid framebuffer
        CtSpritePrivate::recursivePaint(renderer);
        return;
    }

    renderer->bindBuffer(framebuffer);

    const CtList<CtSprite *> &lst = orderedChildren();

    foreach (CtSprite *item, lst) {
        if (item->isVisible())
            item->d_ptr->recursivePaint(renderer);
    }

    renderer->releaseBuffer();

    renderer->begin();
    renderer->m_opacity = relativeOpacity();
    renderer->m_projectionMatrix = currentViewportProjectionMatrix();
    q->paint(renderer);
    renderer->end();
}

void CtSceneFrameBufferPrivate::deleteBuffers()
{
    if (framebuffer > 0) {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }

    if (depthbuffer > 0) {
        glDeleteRenderbuffers(1, &depthbuffer);
        depthbuffer = 0;
    }
}

void CtSceneFrameBufferPrivate::resizeBuffer(int w, int h)
{
    if (w == bufferWidth && h == bufferHeight)
        return;

    bufferWidth = w;
    bufferHeight = h;

    deleteBuffers();
    texture->release();

    if (w <= 0 || h <= 0)
        return;

    texture->loadFromData(w, h, 4, 0);

    GLint defaultFBO;
    CtGL::glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);

    CtGL::glGenFramebuffers(1, &framebuffer);

    // create render buffer object
    CtGL::glGenRenderbuffers(1, &depthbuffer);

    // bind render buffer
    CtGL::glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    // set render buffer storage
    CtGL::glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);

    // bind framebuffer object
    CtGL::glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // attach texture and render buffer
    CtGL::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                 GL_TEXTURE_2D, texture->id(), 0);

    CtGL::glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                    GL_RENDERBUFFER, depthbuffer);

    // check if framebuffer is ready
    GLuint status = CtGL::glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
        CT_WARNING("Could not create framebuffer");

    CtGL::glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
}


CtSceneFrameBuffer::CtSceneFrameBuffer(CtSprite *parent)
    : CtSprite(new CtSceneFrameBufferPrivate(this))
{
    CT_D(CtSceneFrameBuffer);
    d->init(parent);
}

void CtSceneFrameBuffer::paint(CtRenderer *renderer)
{
    CT_D(CtSceneFrameBuffer);
    renderer->drawTexture(d->shaderEffect, d->texture, d->width, d->height);
}

CtShaderEffect *CtSceneFrameBuffer::shaderEffect() const
{
    CT_D(CtSceneFrameBuffer);
    return d->shaderEffect;
}

void CtSceneFrameBuffer::setShaderEffect(CtShaderEffect *effect)
{
    CT_D(CtSceneFrameBuffer);
    d->shaderEffect = effect;
}

bool CtSceneFrameBuffer::isValidBuffer() const
{
    CT_D(CtSceneFrameBuffer);
    return d->texture->isValid();
}

void CtSceneFrameBuffer::setBufferSize(int width, int height)
{
    CT_D(CtSceneFrameBuffer);
    d->resizeBuffer(width, height);
}

/////////////////////////////////////////////////
// CtSceneTextureItem
/////////////////////////////////////////////////

CtSceneTextureItemPrivate::CtSceneTextureItemPrivate(CtSceneTextureItem *q)
    : CtSpritePrivate(q),
      textureAtlasIndex(-1),
      ownTexture(false),
      texture(0),
      shaderEffect(0)
{

}

void CtSceneTextureItemPrivate::init(CtSprite *parent)
{
    CtSpritePrivate::init(parent);

    if (texture) {
        q->setWidth(texture->width());
        q->setHeight(texture->height());
    }

    shaderEffect = ct_sharedTextureShaderEffect();
}

void CtSceneTextureItemPrivate::release()
{
    if (texture && ownTexture) {
        delete texture;
        texture = 0;
    }

    CtSpritePrivate::release();
}


CtSceneTextureItem::CtSceneTextureItem(CtSprite *parent)
    : CtSprite(new CtSceneTextureItemPrivate(this))
{
    CT_D(CtSceneTextureItem);
    d->init(parent);
}

CtSceneTextureItem::CtSceneTextureItem(CtTexture *texture, CtSprite *parent)
    : CtSprite(new CtSceneTextureItemPrivate(this))
{
    CT_D(CtSceneTextureItem);
    d->texture = texture;
    d->init(parent);
}

CtSceneTextureItem::CtSceneTextureItem(CtSceneTextureItemPrivate *dd)
    : CtSprite(dd)
{

}

CtTexture *CtSceneTextureItem::texture() const
{
    CT_D(CtSceneTextureItem);
    return d->texture;
}

void CtSceneTextureItem::setTexture(CtTexture *texture)
{
    CT_D(CtSceneTextureItem);
    if (d->texture == texture)
        return;

    if (d->texture && d->ownTexture) {
        delete d->texture;
        d->texture = 0;
        d->ownTexture = false;
    }

    d->texture = texture;
}

int CtSceneTextureItem::textureAtlasIndex() const
{
    CT_D(CtSceneTextureItem);
    return d->textureAtlasIndex;
}

void CtSceneTextureItem::setTextureAtlasIndex(int index)
{
    CT_D(CtSceneTextureItem);
    d->textureAtlasIndex = index;
}

CtShaderEffect *CtSceneTextureItem::shaderEffect() const
{
    CT_D(CtSceneTextureItem);
    return d->shaderEffect;
}

void CtSceneTextureItem::setShaderEffect(CtShaderEffect *effect)
{
    CT_D(CtSceneTextureItem);
    d->shaderEffect = effect;
}

bool CtSceneTextureItem::load(const CtString &filePath)
{
    CT_D(CtSceneTextureItem);

    if (!d->ownTexture) {
        d->texture = new CtTexture();
        d->ownTexture = true;
    }

    if (!d->texture->load(filePath))
        return false;

    setWidth(d->texture->width());
    setHeight(d->texture->height());

    return true;
}

/////////////////////////////////////////////////
// CtSceneImage
/////////////////////////////////////////////////

CtSceneImagePrivate::CtSceneImagePrivate(CtSceneImage *q)
    : CtSceneTextureItemPrivate(q),
      fillMode(CtSceneImage::Stretch)
{

}

CtSceneImage::CtSceneImage(CtSprite *parent)
    : CtSceneTextureItem(new CtSceneImagePrivate(this))
{
    CT_D(CtSceneImage);
    d->init(parent);
}

CtSceneImage::CtSceneImage(CtTexture *texture, CtSprite *parent)
    : CtSceneTextureItem(new CtSceneImagePrivate(this))
{
    CT_D(CtSceneImage);
    d->texture = texture;
    d->init(parent);
}

CtSceneImage::CtSceneImage(CtSceneImagePrivate *dd)
    : CtSceneTextureItem(dd)
{

}

CtSceneImage::FillMode CtSceneImage::fillMode() const
{
    CT_D(CtSceneImage);
    return d->fillMode;
}

void CtSceneImage::setFillMode(FillMode mode)
{
    CT_D(CtSceneImage);
    d->fillMode = mode;
}

void CtSceneImage::paint(CtRenderer *renderer)
{
    CT_D(CtSceneImage);

    const bool vTile = (d->fillMode == CtSceneImage::Tile ||
                        d->fillMode == CtSceneImage::TileVertically);

    const bool hTile = (d->fillMode == CtSceneImage::Tile ||
                        d->fillMode == CtSceneImage::TileHorizontally);

    renderer->drawTexture(d->shaderEffect, d->texture, d->width, d->height,
                          vTile, hTile, d->textureAtlasIndex);
}

/////////////////////////////////////////////////
// CtSceneImagePoly
/////////////////////////////////////////////////

CtSceneImagePolyPrivate::CtSceneImagePolyPrivate(CtSceneImagePoly *q)
    : CtSceneImagePrivate(q)
{

}

CtSceneImagePoly::CtSceneImagePoly(CtSprite *parent)
    : CtSceneImage(new CtSceneImagePolyPrivate(this))
{
    CT_D(CtSceneImagePoly);
    d->init(parent);
}

CtSceneImagePoly::CtSceneImagePoly(CtTexture *texture, CtSprite *parent)
    : CtSceneImage(new CtSceneImagePolyPrivate(this))
{
    CT_D(CtSceneImagePoly);
    d->texture = texture;
    d->init(parent);
}

void CtSceneImagePoly::paint(CtRenderer *renderer)
{
    CT_D(CtSceneImagePoly);

    const int count = d->vertices.size();

    if (!d->texture || count < 3)
        return;

    if (d->textureAtlasIndex >= 0) {
        CT_WARNING("CtSceneImagePoly does not support atlas texture yet!");
        return;
    }

    const bool vTile = (d->fillMode == CtSceneImagePoly::Tile ||
                        d->fillMode == CtSceneImagePoly::TileVertically);

    const bool hTile = (d->fillMode == CtSceneImagePoly::Tile ||
                        d->fillMode == CtSceneImagePoly::TileHorizontally);

    int i = 0;
    GLfloat *vertices = new GLfloat[count * 2];
    GLfloat *texCoords = new GLfloat[count * 2];

    const ctreal w = hTile ? d->texture->width() : width();
    const ctreal h = vTile ? d->texture->height() : height();

    foreach (const CtPoint &p, d->vertices) {
        vertices[i] = p.x();
        vertices[i + 1] = p.y();

        texCoords[i] = p.x() / w;
        texCoords[i + 1] = p.y() / h;

        i += 2;
    }

    renderer->drawTexture(d->shaderEffect, d->texture, vertices, texCoords, count,
                          vTile, hTile);

    delete [] vertices;
    delete [] texCoords;
}

CtVector<CtPoint> CtSceneImagePoly::vertices() const
{
    CT_D(CtSceneImagePoly);
    return d->vertices;
}

void CtSceneImagePoly::setVertices(const CtVector<CtPoint> &vertices)
{
    CT_D(CtSceneImagePoly);
    d->vertices = vertices;
}


/////////////////////////////////////////////////
// CtSceneFragments
/////////////////////////////////////////////////

CtSceneFragmentsPrivate::CtSceneFragmentsPrivate(CtSceneFragments *q)
    : CtSceneTextureItemPrivate(q)
{

}

void CtSceneFragmentsPrivate::init(CtSprite *parent)
{
    CtSceneTextureItemPrivate::init(parent);
    shaderEffect = ct_sharedFragmentShaderEffect();
}

void CtSceneFragmentsPrivate::release()
{
    CtSceneTextureItemPrivate::release();

    foreach (CtSceneFragments::Fragment *f, fragments)
        delete f;
}


CtSceneFragments::Fragment::Fragment()
    : m_x(0),
      m_y(0),
      m_width(0),
      m_height(0),
      m_opacity(1.0),
      m_atlasIndex(-1),
      m_userData(0)
{

}

void CtSceneFragments::Fragment::setX(ctreal x)
{
    m_x = x;
}

void CtSceneFragments::Fragment::setY(ctreal y)
{
    m_y = y;
}

void CtSceneFragments::Fragment::setWidth(ctreal w)
{
    m_width = w;
}

void CtSceneFragments::Fragment::setHeight(ctreal h)
{
    m_height = h;
}

void CtSceneFragments::Fragment::setAtlasIndex(int index)
{
    m_atlasIndex = index;
}

void CtSceneFragments::Fragment::setUserData(void *data)
{
    m_userData = data;
}


CtSceneFragments::CtSceneFragments(CtSprite *parent)
    : CtSceneTextureItem(new CtSceneFragmentsPrivate(this))
{
    CT_D(CtSceneFragments);
    d->init(parent);
}

CtSceneFragments::CtSceneFragments(CtTexture *texture, CtSprite *parent)
    : CtSceneTextureItem(new CtSceneFragmentsPrivate(this))
{
    CT_D(CtSceneFragments);
    d->texture = texture;
    d->init(parent);
}

CtList<CtSceneFragments::Fragment *> CtSceneFragments::fragments() const
{
    CT_D(CtSceneFragments);
    return d->fragments;
}

void CtSceneFragments::clearFragments()
{
    CT_D(CtSceneFragments);

    foreach (Fragment *f, d->fragments)
        delete f;

    d->fragments.clear();
}

bool CtSceneFragments::appendFragment(Fragment *fragment)
{
    return insertFragment(-1, fragment);
}

bool CtSceneFragments::insertFragment(int index, Fragment *fragment)
{
    CT_D(CtSceneFragments);

    foreach (Fragment *f, d->fragments) {
        if (fragment == f)
            return false;
    }

    if (index < 0 || index >= d->fragments.size()) {
        d->fragments.append(fragment);
    } else {
        CtList<Fragment *>::iterator it;
        it = d->fragments.begin();
        std::advance(it, index);
        d->fragments.insert(it, fragment);
    }

    return true;
}

bool CtSceneFragments::removeFragment(Fragment *fragment)
{
    CT_D(CtSceneFragments);

    CtList<Fragment *>::iterator it;

    for (it = d->fragments.begin(); it != d->fragments.end(); it++) {
        if (*it == fragment) {
            d->fragments.remove(it);
            delete fragment;
            return true;
        }
    }

    return false;
}

void CtSceneFragments::paint(CtRenderer *renderer)
{
    CT_D(CtSceneFragments);

    if (!d->shaderEffect)
        return;

    CtShaderEffect::Element e;
    CtList<CtShaderEffect::Element> elements;

    foreach (CtSceneFragments::Fragment *f, d->fragments) {
        e.x = f->x();
        e.y = f->y();
        e.width = f->width();
        e.height = f->height();
        e.opacity = f->opacity();
        e.textureAtlasIndex = f->atlasIndex();
        elements.append(e);
    }

    renderer->drawElements(d->shaderEffect, d->texture, elements);
}


/////////////////////////////////////////////////
// CtSceneParticleSystem
/////////////////////////////////////////////////

CtSceneParticleSystemPrivate::CtSceneParticleSystemPrivate(CtSceneParticleSystem *q)
    : CtSceneTextureItemPrivate(q),
      vertices(0),
      attrCount(7),
      vertexSize(7 * sizeof(GLfloat)),
      vertexCount(0)
{

}

void CtSceneParticleSystemPrivate::init(CtSprite *parent)
{
    CtSceneTextureItemPrivate::init(parent);
    shaderEffect = ct_sharedParticleShaderEffect();
}

void CtSceneParticleSystemPrivate::release()
{
    CtSceneTextureItemPrivate::release();

    foreach (CtSceneParticleSystem::Particle *f, particles)
        delete f;

    if (vertices) {
        delete [] vertices;
        vertices = 0;
    }
}

void CtSceneParticleSystemPrivate::recreateVertexBuffer()
{
    if (vertices) {
        delete [] vertices;
        vertices = 0;
    }

    vertexCount = particles.size();
    const size_t size = vertexSize * vertexCount;

    if (size > 0)
        vertices = new GLfloat[size];
}


CtSceneParticleSystem::Particle::Particle()
    : m_x(0),
      m_y(0),
      m_size(10),
      m_color(1, 1, 1, 1),
      m_userData(0)
{

}

CtSceneParticleSystem::CtSceneParticleSystem(CtSprite *parent)
    : CtSceneTextureItem(new CtSceneParticleSystemPrivate(this))
{
    CT_D(CtSceneParticleSystem);
    d->init(parent);
}

CtSceneParticleSystem::CtSceneParticleSystem(CtTexture *texture, CtSprite *parent)
    : CtSceneTextureItem(new CtSceneParticleSystemPrivate(this))
{
    CT_D(CtSceneParticleSystem);
    d->texture = texture;
    d->init(parent);
}

CtVector<CtSceneParticleSystem::Particle *> CtSceneParticleSystem::particles() const
{
    CT_D(CtSceneParticleSystem);
    return d->particles;
}

bool CtSceneParticleSystem::addParticle(Particle *particle)
{
    CT_D(CtSceneParticleSystem);

    foreach (Particle *p, d->particles) {
        if (particle == p)
            return false;
    }

    d->particles.append(particle);
    d->recreateVertexBuffer();
    return true;
}

bool CtSceneParticleSystem::removeParticle(Particle *fragment)
{
    CT_D(CtSceneParticleSystem);

    bool found = false;
    CtVector<Particle *>::iterator it;

    for (it = d->particles.begin(); it != d->particles.end(); it++) {
        if (*it == fragment) {
            d->particles.remove(it);
            delete fragment;

            found = true;
            break;
        }
    }

    if (found)
        d->recreateVertexBuffer();

    return found;
}

void CtSceneParticleSystem::clearParticles()
{
    CT_D(CtSceneParticleSystem);

    foreach (Particle *p, d->particles)
        delete p;

    d->particles.clear();
    d->recreateVertexBuffer();
}

void CtSceneParticleSystem::paint(CtRenderer *renderer)
{
    CT_D(CtSceneParticleSystem);

    CtTexture *texture = d->texture;

    if (!texture || !d->vertices || !d->shaderEffect || !d->shaderEffect->init())
        return;

    GLfloat *vptr = d->vertices;

    foreach (CtSceneParticleSystem::Particle *p, d->particles) {
        const CtColor &c = p->color();

        vptr[0] = p->x();
        vptr[1] = p->y();
        vptr[2] = p->pointSize();
        vptr[3] = c.red();
        vptr[4] = c.green();
        vptr[5] = c.blue();
        vptr[6] = c.alpha();

        vptr += d->attrCount;
    }

    // use program
    CtShaderProgram *program = d->shaderEffect->program();

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

    GLfloat *vertexPtr = d->vertices;
    program->setVertexAttributePointer(locPosition, GL_FLOAT, 2, d->vertexSize, vertexPtr);
    vertexPtr += 2;
    program->setVertexAttributePointer(locPointSize, GL_FLOAT, 1, d->vertexSize, vertexPtr);
    vertexPtr += 1;
    program->setVertexAttributePointer(locColor, GL_FLOAT, 4, d->vertexSize, vertexPtr);

    // apply texture
    CtGL::glActiveTexture(GL_TEXTURE0);
    CtGL::glBindTexture(GL_TEXTURE_2D, texture->id());
    program->setUniformValue(locTexture, int(0));
    CtGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    CtGL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    CtGL::glDrawArrays(GL_POINTS, 0, d->vertexCount);

    program->release();
}
