#include "ctitem.h"
#include "ctmath.h"
#include "ctitem_p.h"
#include "ctsceneview.h"
#include "ctdragcursor.h"
#include "ctshadereffect.h"
#include "3rdparty/tricollision.h"


static CtShaderEffect *ct_sharedSolidShaderEffect()
{
    static CtShaderEffect *r = new CtShaderEffect(CtShaderEffect::Solid);
    return r;
}

static CtShaderEffect *ct_sharedTextureShaderEffect()
{
    static CtShaderEffect *r = new CtShaderEffect(CtShaderEffect::Texture);
    return r;
}


CtSceneItemPrivate::CtSceneItemPrivate(CtSceneItem *q)
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
      parent(0),
      scene(0),
      xCenter(0),
      yCenter(0),
      transformDirty(true),
      flags(CtSceneItem::NoFlag),
      pendingDelete(false)
{

}

CtSceneItemPrivate::~CtSceneItemPrivate()
{

}

void CtSceneItemPrivate::init(CtSceneItem *p)
{
    parent = p;
    if (parent)
        parent->d_ptr->addItem(q);
}

void CtSceneItemPrivate::release()
{
    if (parent)
        parent->d_ptr->removeItem(q);

    // delete children
    CtList<CtSceneItem *> oldChildren = children;

    foreach (CtSceneItem *item, oldChildren)
        delete item;
}

ctreal CtSceneItemPrivate::relativeOpacity()
{
    // XXX: optimize
    if (!parent || opacity == 0.0)
        return opacity;
    else
        return opacity * parent->opacity();
}

bool CtSceneItemPrivate::relativeVisible()
{
    // XXX: optimize
    if (!parent)
        return visible;
    else
        return visible && parent->isVisible();
}

void CtSceneItemPrivate::addItem(CtSceneItem *item)
{
    children.remove(item);
    children.push_back(item);

    CtSceneView *mScene = q->scene();

    if (mScene)
        mScene->itemAddedToScene(q);
}

void CtSceneItemPrivate::removeItem(CtSceneItem *item)
{
    children.remove(item);

    CtSceneView *mScene = q->scene();

    if (mScene)
        mScene->itemRemovedFromScene(q);
}

void CtSceneItemPrivate::setScene(CtSceneView *newScene)
{
    scene = newScene;
    // update cache
}

bool CtSceneItemPrivate_sort_compare(CtSceneItem *a, CtSceneItem *b)
{
    return (!a || !b) ? false : (a->z() < b->z());
}

void CtSceneItemPrivate::checkTransformMatrix()
{
    if (!transformDirty)
        return;

    CtMatrix modelMatrix;

    ctreal ox = ctRound(xCenter);
    ctreal oy = ctRound(yCenter);

    // pixel aligned
    ctreal rx = ctRound(x);
    ctreal ry = ctRound(y);

    modelMatrix.translate(rx, ry, 0);

    modelMatrix.translate(ox, oy, 0);
    modelMatrix.scale(xScale, yScale, 1);
    modelMatrix.rotate(-(GLfloat)rotation, 0, 0, 1);
    modelMatrix.translate(-ox, -oy, 0);

    if (!parent) {
        sceneTransformMatrix = modelMatrix;
    } else {
        sceneTransformMatrix = modelMatrix;
        sceneTransformMatrix.multiply(parent->sceneTransformMatrix());
    }

    localTransformMatrix = sceneTransformMatrix;
    localTransformMatrix.invert();

    //transformDirty = true; // XXX: enable for optimization
}

CtMatrix CtSceneItemPrivate::currentSceneTransformMatrix()
{
    checkTransformMatrix();
    return sceneTransformMatrix;
}

CtMatrix CtSceneItemPrivate::currentLocalTransformMatrix()
{
    checkTransformMatrix();
    return localTransformMatrix;
}

CtMatrix CtSceneItemPrivate::currentViewProjectionMatrix()
{
    CtSceneView *sc = q->scene();
    CtMatrix result = currentSceneTransformMatrix();

    if (sc) {
        CtMatrix orthoMatrix;
        orthoMatrix.ortho(0, sc->width(), sc->height(), 0, 1, -1);
        result.multiply(orthoMatrix);
    }

    return result;
}

CtSceneItem::CtSceneItem(CtSceneItem *parent)
    : d_ptr(new CtSceneItemPrivate(this))
{
    CT_D(CtSceneItem);
    d->init(parent);
}

CtSceneItem::CtSceneItem(CtSceneItemPrivate *dd)
    : d_ptr(dd)
{

}

CtSceneItem::~CtSceneItem()
{
    CT_D(CtSceneItem);
    d->release();
    delete d;
}

CtSceneView *CtSceneItem::scene() const
{
    CT_D(CtSceneItem);
    if (d->parent)
        return d->parent->scene();
    else
        return d->scene;
}

CtSceneItem *CtSceneItem::parent() const
{
    CT_D(CtSceneItem);
    return d->parent;
}

ctreal CtSceneItem::x() const
{
    CT_D(CtSceneItem);
    return d->x;
}

void CtSceneItem::setX(ctreal x)
{
    CT_D(CtSceneItem);
    if (d->x == x)
        return;

    d->x = x;

    ChangeValue value;
    value.realValue = x;
    itemChanged(XChange, value);
}

ctreal CtSceneItem::y() const
{
    CT_D(CtSceneItem);
    return d->y;
}

void CtSceneItem::setY(ctreal y)
{
    CT_D(CtSceneItem);
    if (d->y == y)
        return;

    d->y = y;

    ChangeValue value;
    value.realValue = y;
    itemChanged(YChange, value);
}

ctreal CtSceneItem::z() const
{
    CT_D(CtSceneItem);
    return d->z;
}

void CtSceneItem::setZ(ctreal z)
{
    CT_D(CtSceneItem);
    if (d->z == z)
        return;

    d->z = z;

    ChangeValue value;
    value.realValue = z;
    itemChanged(ZChange, value);
}

ctreal CtSceneItem::rotation() const
{
    CT_D(CtSceneItem);
    return d->rotation;
}

void CtSceneItem::setRotation(ctreal rotation)
{
    CT_D(CtSceneItem);
    if (d->rotation == rotation)
        return;

    d->rotation = rotation;

    ChangeValue value;
    value.realValue = rotation;
    itemChanged(RotationChange, value);
}

ctreal CtSceneItem::xScale() const
{
    CT_D(CtSceneItem);
    return d->xScale;
}

void CtSceneItem::setXScale(ctreal scale)
{
    CT_D(CtSceneItem);
    if (d->xScale == scale)
        return;

    d->xScale = scale;

    ChangeValue value;
    value.realValue = scale;
    itemChanged(XScaleChange, value);
}

ctreal CtSceneItem::yScale() const
{
    CT_D(CtSceneItem);
    return d->yScale;
}

void CtSceneItem::setYScale(ctreal scale)
{
    CT_D(CtSceneItem);
    if (d->yScale == scale)
        return;

    d->yScale = scale;

    ChangeValue value;
    value.realValue = scale;
    itemChanged(YScaleChange, value);
}

void CtSceneItem::scale(ctreal xScale, ctreal yScale)
{
    setXScale(xScale);
    setYScale(yScale);
}

ctreal CtSceneItem::opacity() const
{
    CT_D(CtSceneItem);
    return d->relativeOpacity();
}

void CtSceneItem::setOpacity(ctreal opacity)
{
    CT_D(CtSceneItem);
    if (d->opacity == opacity)
        return;

    d->opacity = opacity;

    ChangeValue value;
    value.realValue = opacity;
    itemChanged(OpacityChange, value);
}

int CtSceneItem::flags() const
{
    CT_D(CtSceneItem);
    return d->flags;
}

void CtSceneItem::setFlag(Flag flag, bool enabled)
{
    CT_D(CtSceneItem);
    if (enabled) {
        d->flags |= flag;
    } else {
        if ((d->flags & flag))
            d->flags ^= flag;
    }
}

void CtSceneItem::deleteLater()
{
    CT_D(CtSceneItem);
    d->pendingDelete = true;
}

ctreal CtSceneItem::width() const
{
    CT_D(CtSceneItem);
    return d->width;
}

void CtSceneItem::setWidth(ctreal width)
{
    CT_D(CtSceneItem);
    if (d->width == width)
        return;

    d->width = width;

    ChangeValue value;
    value.realValue = width;
    itemChanged(WidthChange, value);
}

ctreal CtSceneItem::height() const
{
    CT_D(CtSceneItem);
    return d->height;
}

void CtSceneItem::setHeight(ctreal height)
{
    CT_D(CtSceneItem);
    if (d->height == height)
        return;

    d->height = height;

    ChangeValue value;
    value.realValue = height;
    itemChanged(HeightChange, value);
}

bool CtSceneItem::contains(ctreal x, ctreal y)
{
    CT_D(CtSceneItem);
    return (x >= 0 && y >= 0 && x < d->width && y < d->height);
}

void CtSceneItem::paint()
{

}

void CtSceneItem::advance(ctuint ms)
{
    CT_UNUSED(ms);
}

bool CtSceneItem::isVisible() const
{
    CT_D(CtSceneItem);
    return d->relativeVisible();
}

void CtSceneItem::setVisible(bool visible)
{
    CT_D(CtSceneItem);
    if (d->visible == visible)
        return;

    d->visible = visible;

    ChangeValue value;
    value.boolValue = visible;
    itemChanged(VisibilityChange, value);
}

CtRectReal CtSceneItem::boundingRect() const
{
    CT_D(CtSceneItem);
    return CtRectReal(0, 0, d->width, d->height);
}

bool CtSceneItem::setDragCursor(CtDragCursor *drag)
{
    if (!drag || drag->sourceItem() != this)
        return false;

    CtSceneView *itemScene = scene();
    return !itemScene ? false : itemScene->setDragCursor(drag);
}

CtPointReal CtSceneItem::mapToScene(ctreal x, ctreal y) const
{
    ctreal sx, sy;
    sceneTransformMatrix().map(x, y, &sx, &sy);
    return CtPointReal(sx, sy);
}

CtPointReal CtSceneItem::mapToItem(CtSceneItem *item, ctreal x, ctreal y) const
{
    if (!item)
        return CtPointReal();

    ctreal sx, sy, rx, ry;
    sceneTransformMatrix().map(x, y, &sx, &sy);
    item->transformMatrix().map(sx, sy, &rx, &ry);
    return CtPointReal(rx, ry);
}

CtPointReal CtSceneItem::mapFromItem(CtSceneItem *item, ctreal x, ctreal y) const
{
    if (!item)
        return CtPointReal();

    ctreal sx, sy, rx, ry;
    item->sceneTransformMatrix().map(x, y, &sx, &sy);
    transformMatrix().map(sx, sy, &rx, &ry);

    return CtPointReal(rx, ry);
}

bool CtSceneItem::collidesWith(CtSceneItem *item) const
{
    if (!item)
        return false;

    ctreal ap1x, ap1y, ap2x, ap2y, ap3x, ap3y, ap4x, ap4y;
    ctreal bp1x, bp1y, bp2x, bp2y, bp3x, bp3y, bp4x, bp4y;

    // bounding rects
    const CtRectReal &rA = boundingRect();
    const CtRectReal &rB = item->boundingRect();

    // matrix transforms
    const CtMatrix &mA = sceneTransformMatrix();
    const CtMatrix &mB = item->sceneTransformMatrix();

    // map top-left
    mA.map(rA.x(), rA.y(), &ap1x, &ap1y);
    mB.map(rB.x(), rB.y(), &bp1x, &bp1y);

    // map bottom-right
    mA.map(rA.x() + rA.width(), rA.y() + rA.height(), &ap3x, &ap3y);
    mB.map(rB.x() + rB.width(), rB.y() + rB.height(), &bp3x, &bp3y);

    // check simple collisions (no rotation)
    if (rotation() == 0 && item->rotation() == 0) {
        return !(ctMin(ap1x, ap3x) > ctMax(bp1x, bp3x) ||
                 ctMin(ap1y, ap3y) > ctMax(bp1y, bp3y) ||
                 ctMax(ap1x, ap3x) < ctMin(bp1x, bp3x) ||
                 ctMax(ap1y, ap3y) < ctMin(bp1y, bp3y));
    }

    // map top-right
    mA.map(rA.x() + rA.width(), rA.y(), &ap2x, &ap2y);
    mB.map(rB.x() + rB.width(), rB.y(), &bp2x, &bp2y);

    // map bottom-left
    mA.map(rA.x(), rA.y() + rA.height(), &ap4x, &ap4y);
    mB.map(rB.x(), rB.y() + rB.height(), &bp4x, &bp4y);

    // create triangles
    const float V[][3] = {{ap1x, ap1y, 0}, {ap2x, ap2y, 0}, {ap3x, ap3y, 0},
                          {ap1x, ap1y, 0}, {ap3x, ap3y, 0}, {ap4x, ap4y, 0}};

    const float U[][3] = {{bp1x, bp1y, 0}, {bp2x, bp2y, 0}, {bp3x, bp3y, 0},
                           {bp1x, bp1y, 0}, {bp3x, bp3y, 0}, {bp4x, bp4y, 0}};

    // check collisions
    if (tri_tri_intersect(V[0], V[1], V[2], U[0], U[1], U[2])) return true;
    if (tri_tri_intersect(V[0], V[1], V[2], U[3], U[4], U[5])) return true;
    if (tri_tri_intersect(V[3], V[4], V[5], U[0], U[1], U[2])) return true;
    if (tri_tri_intersect(V[3], V[4], V[5], U[3], U[4], U[5])) return true;

    return false;
}

void CtSceneItem::setTransformOrigin(ctreal x, ctreal y)
{
    CT_D(CtSceneItem);
    d->xCenter = x;
    d->yCenter = y;
}

CtList<CtSceneItem *> CtSceneItem::children() const
{
    CT_D(CtSceneItem);
    return d->children;
}

CtMatrix CtSceneItem::transformMatrix() const
{
    CT_D(CtSceneItem);
    return d->currentLocalTransformMatrix();
}

CtMatrix CtSceneItem::sceneTransformMatrix() const
{
    CT_D(CtSceneItem);
    return d->currentSceneTransformMatrix();
}

bool CtSceneItem::event(CtEvent *event)
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
        event->setAccepted(false);
        break;
    }

    return event->isAccepted();
}

void CtSceneItem::mousePressEvent(CtMouseEvent *event)
{
    event->setAccepted(false);
}

void CtSceneItem::mouseMoveEvent(CtMouseEvent *event)
{
    event->setAccepted(false);
}

void CtSceneItem::mouseReleaseEvent(CtMouseEvent *event)
{
    event->setAccepted(false);
}

void CtSceneItem::touchBeginEvent(CtTouchEvent *event)
{
    event->setAccepted(false);
}

void CtSceneItem::touchUpdateEvent(CtTouchEvent *event)
{
    event->setAccepted(false);
}

void CtSceneItem::touchEndEvent(CtTouchEvent *event)
{
    event->setAccepted(false);
}

void CtSceneItem::dragEnterEvent(CtDragDropEvent *event)
{
    event->setAccepted(false);
}

void CtSceneItem::dragMoveEvent(CtDragDropEvent *event)
{
    event->setAccepted(false);
}

void CtSceneItem::dragLeaveEvent(CtDragDropEvent *event)
{
    event->setAccepted(false);
}

void CtSceneItem::dropEvent(CtDragDropEvent *event)
{
    event->setAccepted(false);
}

void CtSceneItem::dragCursorDropEvent(CtDragDropEvent *event)
{
    event->setAccepted(false);
}

void CtSceneItem::dragCursorCancelEvent(CtDragDropEvent *event)
{
    event->setAccepted(false);
}

/////////////////////////////////////////////////
// CtSceneRect
/////////////////////////////////////////////////

CtSceneRectPrivate::CtSceneRectPrivate(CtSceneRect *q)
    : CtSceneItemPrivate(q),
      r(1),
      g(1),
      b(1),
      shaderEffect(0)
{

}

void CtSceneRectPrivate::init(CtSceneItem *parent)
{
    CtSceneItemPrivate::init(parent);
    shaderEffect = ct_sharedSolidShaderEffect();
}

void CtSceneRectPrivate::release()
{
    CtSceneItemPrivate::release();
}

void CtSceneRectPrivate::draw()
{
    if (!shaderEffect || !shaderEffect->init())
        return;

    CtMatrix matrix = currentViewProjectionMatrix();
    shaderEffect->drawSolid(matrix, width, height, r, g, b, 1.0, relativeOpacity());
}


CtSceneRect::CtSceneRect(CtSceneItem *parent)
    : CtSceneItem(new CtSceneRectPrivate(this))
{
    CT_D(CtSceneRect);
    d->init(parent);
}

CtSceneRect::CtSceneRect(ctreal r, ctreal g, ctreal b, CtSceneItem *parent)
    : CtSceneItem(new CtSceneRectPrivate(this))
{
    CT_D(CtSceneRect);
    d->init(parent);
    d->r = r;
    d->g = g;
    d->b = b;
}

void CtSceneRect::paint()
{
    CT_D(CtSceneRect);
    d->draw();
}

ctreal CtSceneRect::r() const
{
    CT_D(CtSceneRect);
    return d->r;
}

ctreal CtSceneRect::g() const
{
    CT_D(CtSceneRect);
    return d->g;
}

ctreal CtSceneRect::b() const
{
    CT_D(CtSceneRect);
    return d->b;
}

void CtSceneRect::setColor(ctreal r, ctreal g, ctreal b)
{
    CT_D(CtSceneRect);
    d->r = r;
    d->g = g;
    d->b = b;
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
// CtSceneTextureItem
/////////////////////////////////////////////////

CtSceneTextureItemPrivate::CtSceneTextureItemPrivate(CtSceneTextureItem *q)
    : CtSceneItemPrivate(q),
      textureAtlasIndex(-1),
      ownTexture(false),
      texture(0),
      shaderEffect(0)
{

}

void CtSceneTextureItemPrivate::init(CtSceneItem *parent)
{
    CtSceneItemPrivate::init(parent);

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

    CtSceneItemPrivate::release();
}


CtSceneTextureItem::CtSceneTextureItem(CtSceneItem *parent)
    : CtSceneItem(new CtSceneTextureItemPrivate(this))
{
    CT_D(CtSceneTextureItem);
    d->init(parent);
}

CtSceneTextureItem::CtSceneTextureItem(CtTexture *texture, CtSceneItem *parent)
    : CtSceneItem(new CtSceneTextureItemPrivate(this))
{
    CT_D(CtSceneTextureItem);
    d->texture = texture;
    d->init(parent);
}

CtSceneTextureItem::CtSceneTextureItem(CtSceneTextureItemPrivate *dd)
    : CtSceneItem(dd)
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
    int pos = filePath.rfind(".");
    CtString ext = pos >= 0 ? filePath.substr(pos) : "";

    if (ext == ".pvr")
        return load(filePath, Ct::PVRTextureFile);
    else if (ext == ".dds")
        return load(filePath, Ct::DDSTextureFile);
    else if (ext == ".tga")
        return load(filePath, Ct::TGATextureFile);

    return false;
}

bool CtSceneTextureItem::load(const CtString &filePath, Ct::TextureFileType type)
{
    CT_D(CtSceneTextureItem);

    if (!d->ownTexture) {
        d->texture = new CtTexture();
        d->ownTexture = true;
    }

    switch (type) {
    case Ct::PVRTextureFile:
        if (!d->texture->loadPVR(filePath))
            return false;
        break;
    case Ct::DDSTextureFile:
        if (!d->texture->loadDDS(filePath))
            return false;
        break;
    case Ct::TGATextureFile:
        if (!d->texture->loadTGA(filePath))
            return false;
        break;
    default:
        return false;
    }

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

void CtSceneImagePrivate::draw()
{
    if (!shaderEffect || !shaderEffect->init())
        return;

    CtMatrix matrix = currentViewProjectionMatrix();
    bool vTile = (fillMode == CtSceneImage::Tile || fillMode == CtSceneImage::TileVertically);
    bool hTile = (fillMode == CtSceneImage::Tile || fillMode == CtSceneImage::TileHorizontally);

    shaderEffect->drawTexture(matrix, texture, width, height, relativeOpacity(), vTile, hTile, textureAtlasIndex);
}


CtSceneImage::CtSceneImage(CtSceneItem *parent)
    : CtSceneTextureItem(new CtSceneImagePrivate(this))
{
    CT_D(CtSceneImage);
    d->init(parent);
}

CtSceneImage::CtSceneImage(CtTexture *texture, CtSceneItem *parent)
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

void CtSceneImage::paint()
{
    CT_D(CtSceneImage);
    d->draw();
}

/////////////////////////////////////////////////
// CtSceneFragments
/////////////////////////////////////////////////

CtSceneFragmentsPrivate::CtSceneFragmentsPrivate(CtSceneFragments *q)
    : CtSceneTextureItemPrivate(q)
{

}

void CtSceneFragmentsPrivate::release()
{
    CtSceneTextureItemPrivate::release();

    foreach (CtSceneFragments::Fragment *f, fragments)
        delete f;
}

void CtSceneFragmentsPrivate::draw()
{
    if (!shaderEffect || !shaderEffect->init())
        return;

    CtMatrix matrix = currentViewProjectionMatrix();

    CtShaderEffect::Element e;
    CtList<CtShaderEffect::Element> elements;

    foreach (CtSceneFragments::Fragment *f, fragments) {
        e.x = f->x();
        e.y = f->y();
        e.width = f->width();
        e.height = f->height();
        e.textureAtlasIndex = f->atlasIndex();
        elements.push_back(e);
    }

    shaderEffect->drawElements(matrix, texture, relativeOpacity(), false, false, elements);
}


CtSceneFragments::Fragment::Fragment()
    : m_x(0),
      m_y(0),
      m_width(0),
      m_height(0),
      m_atlasIndex(-1),
      m_userData(0)
{

}

void CtSceneFragments::paint()
{
    CT_D(CtSceneFragments);
    d->draw();
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


CtSceneFragments::CtSceneFragments(CtSceneItem *parent)
    : CtSceneTextureItem(new CtSceneFragmentsPrivate(this))
{
    CT_D(CtSceneFragments);
    d->init(parent);
}

CtSceneFragments::CtSceneFragments(CtTexture *texture, CtSceneItem *parent)
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
        d->fragments.push_back(fragment);
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
            d->fragments.erase(it);
            delete fragment;
            return true;
        }
    }

    return false;
}
