#include "flitem.h"
#include "flmath.h"
#include "flitem_p.h"
#include "flsceneview.h"
#include "fldragcursor.h"
#include "flshadereffect.h"
#include "3rdparty/tricollision.h"


static FlShaderEffect *fl_sharedSolidShaderEffect()
{
    static FlShaderEffect *r = new FlShaderEffect(FlShaderEffect::Solid);
    return r;
}

static FlShaderEffect *fl_sharedTextureShaderEffect()
{
    static FlShaderEffect *r = new FlShaderEffect(FlShaderEffect::Texture);
    return r;
}


FlSceneItemPrivate::FlSceneItemPrivate(FlSceneItem *q)
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
      flags(FlSceneItem::NoFlag)
{

}

void FlSceneItemPrivate::init(FlSceneItem *p)
{
    parent = p;
    if (parent)
        parent->d_ptr->addItem(q);
}

void FlSceneItemPrivate::release()
{
    if (parent)
        parent->d_ptr->removeItem(q);

    // delete children
    FlList<FlSceneItem *> oldChildren = children;

    foreach (FlSceneItem *item, oldChildren)
        delete item;
}

void FlSceneItemPrivate::addItem(FlSceneItem *item)
{
    children.remove(item);
    children.push_back(item);

    FlSceneView *mScene = q->scene();

    if (mScene)
        mScene->itemAddedToScene(q);
}

void FlSceneItemPrivate::removeItem(FlSceneItem *item)
{
    children.remove(item);

    FlSceneView *mScene = q->scene();

    if (mScene)
        mScene->itemRemovedFromScene(q);
}

void FlSceneItemPrivate::setScene(FlSceneView *newScene)
{
    scene = newScene;
    // update cache
}

bool FlSceneItemPrivate_sort_compare(FlSceneItem *a, FlSceneItem *b)
{
    return (!a || !b) ? false : (a->z() < b->z());
}

void FlSceneItemPrivate::checkTransformMatrix()
{
    if (!transformDirty)
        return;

    FlMatrix modelMatrix;
    flreal ox = xCenter;
    flreal oy = yCenter;

    modelMatrix.translate(x, y, 0);

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

FlMatrix FlSceneItemPrivate::currentSceneTransformMatrix()
{
    checkTransformMatrix();
    return sceneTransformMatrix;
}

FlMatrix FlSceneItemPrivate::currentLocalTransformMatrix()
{
    checkTransformMatrix();
    return localTransformMatrix;
}

FlMatrix FlSceneItemPrivate::currentViewProjectionMatrix()
{
    FlSceneView *sc = q->scene();
    FlMatrix result = currentSceneTransformMatrix();

    if (sc) {
        FlMatrix orthoMatrix;
        orthoMatrix.ortho(0, sc->width(), sc->height(), 0, 1, -1);
        result.multiply(orthoMatrix);
    }

    return result;
}

FlSceneItem::FlSceneItem(FlSceneItem *parent)
    : d_ptr(new FlSceneItemPrivate(this))
{
    FL_D(FlSceneItem);
    d->init(parent);
}

FlSceneItem::FlSceneItem(FlSceneItemPrivate *dd)
    : d_ptr(dd)
{

}

FlSceneItem::~FlSceneItem()
{
    FL_D(FlSceneItem);
    d->release();
    delete d;
}

FlSceneView *FlSceneItem::scene() const
{
    FL_D(FlSceneItem);
    if (d->parent)
        return d->parent->scene();
    else
        return d->scene;
}

FlSceneItem *FlSceneItem::parent() const
{
    FL_D(FlSceneItem);
    return d->parent;
}

flreal FlSceneItem::x() const
{
    FL_D(FlSceneItem);
    return d->x;
}

void FlSceneItem::setX(flreal x)
{
    FL_D(FlSceneItem);
    if (d->x == x)
        return;

    d->x = x;

    ChangeValue value;
    value.realValue = x;
    itemChanged(XChange, value);
}

flreal FlSceneItem::y() const
{
    FL_D(FlSceneItem);
    return d->y;
}

void FlSceneItem::setY(flreal y)
{
    FL_D(FlSceneItem);
    if (d->y == y)
        return;

    d->y = y;

    ChangeValue value;
    value.realValue = y;
    itemChanged(YChange, value);
}

flreal FlSceneItem::z() const
{
    FL_D(FlSceneItem);
    return d->z;
}

void FlSceneItem::setZ(flreal z)
{
    FL_D(FlSceneItem);
    if (d->z == z)
        return;

    d->z = z;

    ChangeValue value;
    value.realValue = z;
    itemChanged(ZChange, value);
}

flreal FlSceneItem::rotation() const
{
    FL_D(FlSceneItem);
    return d->rotation;
}

void FlSceneItem::setRotation(flreal rotation)
{
    FL_D(FlSceneItem);
    if (d->rotation == rotation)
        return;

    d->rotation = rotation;

    ChangeValue value;
    value.realValue = rotation;
    itemChanged(RotationChange, value);
}

flreal FlSceneItem::xScale() const
{
    FL_D(FlSceneItem);
    return d->xScale;
}

void FlSceneItem::setXScale(flreal scale)
{
    FL_D(FlSceneItem);
    if (d->xScale == scale)
        return;

    d->xScale = scale;

    ChangeValue value;
    value.realValue = scale;
    itemChanged(XScaleChange, value);
}

flreal FlSceneItem::yScale() const
{
    FL_D(FlSceneItem);
    return d->yScale;
}

void FlSceneItem::setYScale(flreal scale)
{
    FL_D(FlSceneItem);
    if (d->yScale == scale)
        return;

    d->yScale = scale;

    ChangeValue value;
    value.realValue = scale;
    itemChanged(YScaleChange, value);
}

void FlSceneItem::scale(flreal xScale, flreal yScale)
{
    setXScale(xScale);
    setYScale(yScale);
}

flreal FlSceneItem::opacity() const
{
    FL_D(FlSceneItem);
    return d->opacity;
}

void FlSceneItem::setOpacity(flreal opacity)
{
    FL_D(FlSceneItem);
    if (d->opacity == opacity)
        return;

    d->opacity = opacity;

    ChangeValue value;
    value.realValue = opacity;
    itemChanged(OpacityChange, value);
}

int FlSceneItem::flags() const
{
    FL_D(FlSceneItem);
    return d->flags;
}

void FlSceneItem::setFlag(Flag flag, bool enabled)
{
    FL_D(FlSceneItem);
    if (enabled) {
        d->flags |= flag;
    } else {
        if ((d->flags & flag))
            d->flags ^= flag;
    }
}

flreal FlSceneItem::width() const
{
    FL_D(FlSceneItem);
    return d->width;
}

void FlSceneItem::setWidth(flreal width)
{
    FL_D(FlSceneItem);
    if (d->width == width)
        return;

    d->width = width;

    ChangeValue value;
    value.realValue = width;
    itemChanged(WidthChange, value);
}

flreal FlSceneItem::height() const
{
    FL_D(FlSceneItem);
    return d->height;
}

void FlSceneItem::setHeight(flreal height)
{
    FL_D(FlSceneItem);
    if (d->height == height)
        return;

    d->height = height;

    ChangeValue value;
    value.realValue = height;
    itemChanged(HeightChange, value);
}

bool FlSceneItem::contains(flreal x, flreal y)
{
    FL_D(FlSceneItem);
    return (x >= 0 && y >= 0 && x < d->width && y < d->height);
}

void FlSceneItem::paint()
{

}

void FlSceneItem::advance(fluint ms)
{
    FL_UNUSED(ms);
}

bool FlSceneItem::isVisible() const
{
    FL_D(FlSceneItem);
    return d->visible;
}

void FlSceneItem::setVisible(bool visible)
{
    FL_D(FlSceneItem);
    if (d->visible == visible)
        return;

    d->visible = visible;

    ChangeValue value;
    value.boolValue = visible;
    itemChanged(VisibilityChange, value);
}

FlRectReal FlSceneItem::boundingRect() const
{
    FL_D(FlSceneItem);
    return FlRectReal(0, 0, d->width, d->height);
}

bool FlSceneItem::setDragCursor(FlDragCursor *drag)
{
    if (!drag || drag->sourceItem() != this)
        return false;

    FlSceneView *itemScene = scene();
    return !itemScene ? false : itemScene->setDragCursor(drag);
}

FlPointReal FlSceneItem::mapToScene(flreal x, flreal y) const
{
    flreal sx, sy;
    sceneTransformMatrix().map(x, y, &sx, &sy);
    return FlPointReal(sx, sy);
}

FlPointReal FlSceneItem::mapToItem(FlSceneItem *item, flreal x, flreal y) const
{
    if (!item)
        return FlPointReal();

    flreal sx, sy, rx, ry;
    sceneTransformMatrix().map(x, y, &sx, &sy);
    item->transformMatrix().map(sx, sy, &rx, &ry);
    return FlPointReal(rx, ry);
}

FlPointReal FlSceneItem::mapFromItem(FlSceneItem *item, flreal x, flreal y) const
{
    if (!item)
        return FlPointReal();

    flreal sx, sy, rx, ry;
    item->sceneTransformMatrix().map(x, y, &sx, &sy);
    transformMatrix().map(sx, sy, &rx, &ry);

    return FlPointReal(rx, ry);
}

bool FlSceneItem::collidesWith(FlSceneItem *item) const
{
    if (!item)
        return false;

    flreal ap1x, ap1y, ap2x, ap2y, ap3x, ap3y, ap4x, ap4y;
    flreal bp1x, bp1y, bp2x, bp2y, bp3x, bp3y, bp4x, bp4y;

    // bounding rects
    const FlRectReal &rA = boundingRect();
    const FlRectReal &rB = item->boundingRect();

    // matrix transforms
    const FlMatrix &mA = sceneTransformMatrix();
    const FlMatrix &mB = item->sceneTransformMatrix();

    // map top-left
    mA.map(rA.x(), rA.y(), &ap1x, &ap1y);
    mB.map(rB.x(), rB.y(), &bp1x, &bp1y);

    // map bottom-right
    mA.map(rA.x() + rA.width(), rA.y() + rA.height(), &ap3x, &ap3y);
    mB.map(rB.x() + rB.width(), rB.y() + rB.height(), &bp3x, &bp3y);

    // check simple collisions (no rotation)
    if (rotation() == 0 && item->rotation() == 0) {
        return !(flMin(ap1x, ap3x) > flMax(bp1x, bp3x) ||
                 flMin(ap1y, ap3y) > flMax(bp1y, bp3y) ||
                 flMax(ap1x, ap3x) < flMin(bp1x, bp3x) ||
                 flMax(ap1y, ap3y) < flMin(bp1y, bp3y));
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

void FlSceneItem::setTransformOrigin(flreal x, flreal y)
{
    FL_D(FlSceneItem);
    d->xCenter = x;
    d->yCenter = y;
}

FlList<FlSceneItem *> FlSceneItem::children() const
{
    FL_D(FlSceneItem);
    return d->children;
}

FlMatrix FlSceneItem::transformMatrix() const
{
    FL_D(FlSceneItem);
    return d->currentLocalTransformMatrix();
}

FlMatrix FlSceneItem::sceneTransformMatrix() const
{
    FL_D(FlSceneItem);
    return d->currentSceneTransformMatrix();
}

bool FlSceneItem::event(FlEvent *event)
{
    event->setAccepted(true);

    switch (event->type()) {
    case FlEvent::MousePress:
        mousePressEvent(static_cast<FlMouseEvent *>(event));
        break;
    case FlEvent::MouseMove:
        mouseMoveEvent(static_cast<FlMouseEvent *>(event));
        break;
    case FlEvent::MouseRelease:
        mouseReleaseEvent(static_cast<FlMouseEvent *>(event));
        break;
    case FlEvent::TouchBegin:
        touchBeginEvent(static_cast<FlTouchEvent *>(event));
        break;
    case FlEvent::TouchUpdate:
        touchUpdateEvent(static_cast<FlTouchEvent *>(event));
        break;
    case FlEvent::TouchEnd:
        touchEndEvent(static_cast<FlTouchEvent *>(event));
        break;
    case FlEvent::DragEnter:
        dragEnterEvent(static_cast<FlDragDropEvent *>(event));
        break;
    case FlEvent::DragMove:
        dragMoveEvent(static_cast<FlDragDropEvent *>(event));
        break;
    case FlEvent::DragLeave:
        dragLeaveEvent(static_cast<FlDragDropEvent *>(event));
        break;
    case FlEvent::Drop:
        dropEvent(static_cast<FlDragDropEvent *>(event));
        break;
    case FlEvent::DragCursorDrop:
        dragCursorDropEvent(static_cast<FlDragDropEvent *>(event));
        break;
    case FlEvent::DragCursorCancel:
        dragCursorCancelEvent(static_cast<FlDragDropEvent *>(event));
        break;
    default:
        event->setAccepted(false);
        break;
    }

    return event->isAccepted();
}

void FlSceneItem::mousePressEvent(FlMouseEvent *event)
{
    event->setAccepted(false);
}

void FlSceneItem::mouseMoveEvent(FlMouseEvent *event)
{
    event->setAccepted(false);
}

void FlSceneItem::mouseReleaseEvent(FlMouseEvent *event)
{
    event->setAccepted(false);
}

void FlSceneItem::touchBeginEvent(FlTouchEvent *event)
{
    event->setAccepted(false);
}

void FlSceneItem::touchUpdateEvent(FlTouchEvent *event)
{
    event->setAccepted(false);
}

void FlSceneItem::touchEndEvent(FlTouchEvent *event)
{
    event->setAccepted(false);
}

void FlSceneItem::dragEnterEvent(FlDragDropEvent *event)
{
    event->setAccepted(false);
}

void FlSceneItem::dragMoveEvent(FlDragDropEvent *event)
{
    event->setAccepted(false);
}

void FlSceneItem::dragLeaveEvent(FlDragDropEvent *event)
{
    event->setAccepted(false);
}

void FlSceneItem::dropEvent(FlDragDropEvent *event)
{
    event->setAccepted(false);
}

void FlSceneItem::dragCursorDropEvent(FlDragDropEvent *event)
{
    event->setAccepted(false);
}

void FlSceneItem::dragCursorCancelEvent(FlDragDropEvent *event)
{
    event->setAccepted(false);
}

/////////////////////////////////////////////////
// FlSceneRect
/////////////////////////////////////////////////

FlSceneRectPrivate::FlSceneRectPrivate(FlSceneRect *q)
    : FlSceneItemPrivate(q),
      r(1),
      g(1),
      b(1),
      shaderEffect(0)
{

}

void FlSceneRectPrivate::init(FlSceneItem *parent)
{
    FlSceneItemPrivate::init(parent);
    shaderEffect = fl_sharedSolidShaderEffect();
}

void FlSceneRectPrivate::release()
{
    FlSceneItemPrivate::release();
}

void FlSceneRectPrivate::draw()
{
    if (!shaderEffect || !shaderEffect->init())
        return;

    FlMatrix matrix = currentViewProjectionMatrix();
    shaderEffect->drawSolid(matrix, width, height, r, g, b, 1.0, opacity);
}


FlSceneRect::FlSceneRect(FlSceneItem *parent)
    : FlSceneItem(new FlSceneRectPrivate(this))
{
    FL_D(FlSceneRect);
    d->init(parent);
}

FlSceneRect::FlSceneRect(flreal r, flreal g, flreal b, FlSceneItem *parent)
    : FlSceneItem(new FlSceneRectPrivate(this))
{
    FL_D(FlSceneRect);
    d->init(parent);
    d->r = r;
    d->g = g;
    d->b = b;
}

void FlSceneRect::paint()
{
    FL_D(FlSceneRect);
    d->draw();
}

flreal FlSceneRect::r() const
{
    FL_D(FlSceneRect);
    return d->r;
}

flreal FlSceneRect::g() const
{
    FL_D(FlSceneRect);
    return d->g;
}

flreal FlSceneRect::b() const
{
    FL_D(FlSceneRect);
    return d->b;
}

void FlSceneRect::setColor(flreal r, flreal g, flreal b)
{
    FL_D(FlSceneRect);
    d->r = r;
    d->g = g;
    d->b = b;
}

FlShaderEffect *FlSceneRect::shaderEffect() const
{
    FL_D(FlSceneRect);
    return d->shaderEffect;
}

void FlSceneRect::setShaderEffect(FlShaderEffect *effect)
{
    FL_D(FlSceneRect);
    d->shaderEffect = effect;
}

/////////////////////////////////////////////////
// FlSceneImage
/////////////////////////////////////////////////

FlSceneImagePrivate::FlSceneImagePrivate(FlSceneImage *q)
    : FlSceneItemPrivate(q),
      textureAtlasIndex(-1),
      ownTexture(false),
      texture(0),
      shaderEffect(0),
      fillMode(FlSceneImage::Stretch)
{

}

void FlSceneImagePrivate::init(FlSceneItem *parent)
{
    FlSceneItemPrivate::init(parent);

    if (texture) {
        q->setWidth(texture->width());
        q->setHeight(texture->height());
    }

    shaderEffect = fl_sharedTextureShaderEffect();
}

void FlSceneImagePrivate::release()
{
    if (texture && ownTexture) {
        delete texture;
        texture = 0;
    }

    FlSceneItemPrivate::release();
}

void FlSceneImagePrivate::draw()
{
    if (!shaderEffect || !shaderEffect->init())
        return;

    FlMatrix matrix = currentViewProjectionMatrix();
    bool vTile = (fillMode == FlSceneImage::Tile || fillMode == FlSceneImage::TileVertically);
    bool hTile = (fillMode == FlSceneImage::Tile || fillMode == FlSceneImage::TileHorizontally);

    shaderEffect->drawTexture(matrix, texture, width, height, opacity, vTile, hTile, textureAtlasIndex);
}


FlSceneImage::FlSceneImage(FlSceneItem *parent)
    : FlSceneItem(new FlSceneImagePrivate(this))
{
    FL_D(FlSceneImage);
    d->init(parent);
}

FlSceneImage::FlSceneImage(FlTexture *texture, FlSceneItem *parent)
    : FlSceneItem(new FlSceneImagePrivate(this))
{
    FL_D(FlSceneImage);
    d->texture = texture;
    d->init(parent);
}

FlSceneImage::FlSceneImage(FlSceneImagePrivate *dd)
    : FlSceneItem(dd)
{

}

void FlSceneImage::paint()
{
    FL_D(FlSceneImage);
    d->draw();
}

FlSceneImage::FillMode FlSceneImage::fillMode() const
{
    FL_D(FlSceneImage);
    return d->fillMode;
}

void FlSceneImage::setFillMode(FillMode mode)
{
    FL_D(FlSceneImage);
    d->fillMode = mode;
}

FlTexture *FlSceneImage::texture() const
{
    FL_D(FlSceneImage);
    return d->texture;
}

void FlSceneImage::setTexture(FlTexture *texture)
{
    FL_D(FlSceneImage);
    if (d->texture == texture)
        return;

    if (d->texture && d->ownTexture) {
        delete d->texture;
        d->texture = 0;
        d->ownTexture = false;
    }

    d->texture = texture;
}

int FlSceneImage::textureAtlasIndex() const
{
    FL_D(FlSceneImage);
    return d->textureAtlasIndex;
}

void FlSceneImage::setTextureAtlasIndex(int index)
{
    FL_D(FlSceneImage);
    d->textureAtlasIndex = index;
}

FlShaderEffect *FlSceneImage::shaderEffect() const
{
    FL_D(FlSceneImage);
    return d->shaderEffect;
}

void FlSceneImage::setShaderEffect(FlShaderEffect *effect)
{
    FL_D(FlSceneImage);
    d->shaderEffect = effect;
}

bool FlSceneImage::load(const FlString &filePath)
{
    int pos = filePath.rfind(".");
    FlString ext = pos >= 0 ? filePath.substr(pos) : "";

    if (ext == ".pvr")
        return load(filePath, Fl::PVRTextureFile);
    else if (ext == ".dds")
        return load(filePath, Fl::DDSTextureFile);
    else if (ext == ".tga")
        return load(filePath, Fl::TGATextureFile);

    return false;
}

bool FlSceneImage::load(const FlString &filePath, Fl::TextureFileType type)
{
    FL_D(FlSceneImage);

    if (!d->ownTexture) {
        d->texture = new FlTexture();
        d->ownTexture = true;
    }

    switch (type) {
    case Fl::PVRTextureFile:
        if (!d->texture->loadPVR(filePath))
            return false;
        break;
    case Fl::DDSTextureFile:
        if (!d->texture->loadDDS(filePath))
            return false;
        break;
    case Fl::TGATextureFile:
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
// FlSceneFragments
/////////////////////////////////////////////////

FlSceneFragmentsPrivate::FlSceneFragmentsPrivate(FlSceneImage *q)
    : FlSceneImagePrivate(q)
{

}

void FlSceneFragmentsPrivate::release()
{
    FlSceneImagePrivate::release();

    foreach (FlSceneFragments::Fragment *f, fragments)
        delete f;
}

void FlSceneFragmentsPrivate::draw()
{
    if (!shaderEffect || !shaderEffect->init())
        return;

    FlMatrix matrix = currentViewProjectionMatrix();
    bool vTile = (fillMode == FlSceneImage::Tile || fillMode == FlSceneImage::TileVertically);
    bool hTile = (fillMode == FlSceneImage::Tile || fillMode == FlSceneImage::TileHorizontally);

    FlShaderEffect::Element e;
    FlList<FlShaderEffect::Element> elements;

    foreach (FlSceneFragments::Fragment *f, fragments) {
        e.x = f->x();
        e.y = f->y();
        e.width = f->width();
        e.height = f->height();
        e.textureAtlasIndex = f->atlasIndex();
        elements.push_back(e);
    }

    shaderEffect->drawElements(matrix, texture, opacity, vTile, hTile, elements);
}


FlSceneFragments::Fragment::Fragment()
    : m_x(0),
      m_y(0),
      m_width(0),
      m_height(0),
      m_atlasIndex(-1),
      m_userData(0)
{

}

void FlSceneFragments::Fragment::setX(flreal x)
{
    m_x = x;
}

void FlSceneFragments::Fragment::setY(flreal y)
{
    m_y = y;
}

void FlSceneFragments::Fragment::setWidth(flreal w)
{
    m_width = w;
}

void FlSceneFragments::Fragment::setHeight(flreal h)
{
    m_height = h;
}

void FlSceneFragments::Fragment::setAtlasIndex(int index)
{
    m_atlasIndex = index;
}

void FlSceneFragments::Fragment::setUserData(void *data)
{
    m_userData = data;
}


FlSceneFragments::FlSceneFragments(FlSceneItem *parent)
    : FlSceneImage(new FlSceneFragmentsPrivate(this))
{
    FL_D(FlSceneFragments);
    d->init(parent);
}

FlSceneFragments::FlSceneFragments(FlTexture *texture, FlSceneItem *parent)
    : FlSceneImage(new FlSceneFragmentsPrivate(this))
{
    FL_D(FlSceneFragments);
    d->texture = texture;
    d->init(parent);
}

void FlSceneFragments::paint()
{
    FL_D(FlSceneFragments);
    d->draw();
}

FlList<FlSceneFragments::Fragment *> FlSceneFragments::fragments() const
{
    FL_D(FlSceneFragments);
    return d->fragments;
}

void FlSceneFragments::clearFragments()
{
    FL_D(FlSceneFragments);

    foreach (Fragment *f, d->fragments)
        delete f;

    d->fragments.clear();
}

bool FlSceneFragments::appendFragment(Fragment *fragment)
{
    return insertFragment(-1, fragment);
}

bool FlSceneFragments::insertFragment(int index, Fragment *fragment)
{
    FL_D(FlSceneFragments);

    foreach (Fragment *f, d->fragments) {
        if (fragment == f)
            return false;
    }

    if (index < 0 || index >= d->fragments.size()) {
        d->fragments.push_back(fragment);
    } else {
        FlList<Fragment *>::iterator it;
        it = d->fragments.begin();
        std::advance(it, index);
        d->fragments.insert(it, fragment);
    }

    return true;
}

bool FlSceneFragments::removeFragment(Fragment *fragment)
{
    FL_D(FlSceneFragments);

    FlList<Fragment *>::iterator it;

    for (it = d->fragments.begin(); it != d->fragments.end(); it++) {
        if (*it == fragment) {
            d->fragments.erase(it);
            delete fragment;
            return true;
        }
    }

    return false;
}
