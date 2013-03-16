#include "ctsceneview.h"
#include "ctitem.h"
#include "ctitem_p.h"
#include "ctrenderer.h"
#include "ctdragcursor.h"
#include "ctwindow_p.h"
#include "ctopenglfunctions.h"

/******************************************************************************
 * CtSceneViewData
 *****************************************************************************/

struct CtSceneViewData
{
    CtSceneViewData();

    void checkSortedItems();

    void updateDragCursor(CtSprite *source);
    void releaseDragCursor(CtSprite *source, bool canceled);

    bool deliverMousePress(CtMouseEvent *event);
    bool deliverMouseMove(CtMouseEvent *event);
    bool deliverMouseRelease(CtMouseEvent *event);

    bool deliverTouchBegin(CtTouchEvent *event);
    bool deliverTouchUpdate(CtTouchEvent *event);
    bool deliverTouchEnd(CtTouchEvent *event);

    CtTouchPointList mapTouchPoints(CtSprite *item, const CtTouchPointList &points) const;

    CtDragCursor *drag;
    CtSprite *rootItem;
    CtList<CtSprite *> sortedItems;
    bool sortedItemsDirty;
    CtSprite *touchGrabber;
    CtSprite *mouseGrabber;
    CtSprite *dragGrabber;
};

CtSceneViewData::CtSceneViewData()
    : drag(0),
      rootItem(0),
      sortedItemsDirty(true),
      touchGrabber(0),
      mouseGrabber(0),
      dragGrabber(0)
{

}

void CtSceneViewData::checkSortedItems()
{
    if (!sortedItemsDirty)
        return;

    sortedItems.clear();

    if (rootItem)
        rootItem->d_ptr->fillItems(sortedItems);

    sortedItemsDirty = false;
}

void CtSceneViewData::updateDragCursor(CtSprite *source)
{
    if (!drag || !drag->sourceItem() || drag->sourceItem() != source)
        return;

    const ctreal x = drag->x();
    const ctreal y = drag->y();
    const CtDragDropEvent::Operation op = (CtDragDropEvent::Operation)drag->operation();

    checkSortedItems();

    CtSprite *currentGrabber = 0;

    foreach_reverse(CtSprite *item, sortedItems) {
        if (!item->isVisible() || !(item->flags() & CtSprite::AcceptsDragEvent))
            continue;

        const CtMatrix &matrix = item->transformMatrix();
        const CtPoint &p1 = matrix.map(x, y);

        if (!item->contains(p1.x(), p1.y()))
            continue;

        bool isNew = (dragGrabber != item);
        CtDragDropEvent ev(isNew ? CtEvent::DragEnter : CtEvent::DragMove,
                           drag->sourceItem(), item, drag->draggedItem(),
                           drag->mime(), op, p1.x(), p1.y(), x, y);

        if (!isNew) {
            item->event(&ev);
            currentGrabber = item;
            break;
        } else {
            if (!item->event(&ev)) {
                continue;
            } else {
                currentGrabber = item;
                break;
            }
        }
    }

    // leave old grabber
    if (dragGrabber && currentGrabber != dragGrabber) {
        const CtMatrix &matrix = dragGrabber->transformMatrix();
        const CtPoint &p1 = matrix.map(x, y);

        CtDragDropEvent ev(CtEvent::DragLeave, drag->sourceItem(), dragGrabber,
                           drag->draggedItem(), drag->mime(), op, p1.x(), p1.y(), x, y);
        dragGrabber->event(&ev);
    }

    dragGrabber = currentGrabber;
}

void CtSceneViewData::releaseDragCursor(CtSprite *source, bool canceled)
{
    if (!drag || !drag->sourceItem() || drag->sourceItem() != source)
        return;

    CtPoint p1;
    const ctreal x = drag->x();
    const ctreal y = drag->y();
    const CtDragDropEvent::Operation op = (CtDragDropEvent::Operation)(drag->operation());

    if (dragGrabber)
        p1 = dragGrabber->transformMatrix().map(x, y);

    if (!dragGrabber || canceled) {
        if (dragGrabber) {
            CtDragDropEvent ev(CtEvent::DragLeave, drag->sourceItem(), dragGrabber,
                               drag->draggedItem(), drag->mime(), op, p1.x(), p1.y(), x, y);
            dragGrabber->event(&ev);
        }

        CtDragDropEvent evc(CtEvent::DragCursorCancel, drag->sourceItem(), dragGrabber,
                            drag->draggedItem(), drag->mime(), op, p1.x(), p1.y(), x, y);
        drag->sourceItem()->event(&evc);
    } else {
        CtDragDropEvent ev(CtEvent::Drop, drag->sourceItem(), dragGrabber,
                           drag->draggedItem(), drag->mime(), op, p1.x(), p1.y(), x, y);
        bool ok = dragGrabber->event(&ev);

        CtDragDropEvent evc(ok ? CtEvent::DragCursorDrop : CtEvent::DragCursorCancel,
                            drag->sourceItem(), dragGrabber, drag->draggedItem(),
                            drag->mime(), op, p1.x(), p1.y(), x, y);
        drag->sourceItem()->event(&evc);
    }

    drag = 0;
    dragGrabber = 0;
}

bool CtSceneViewData::deliverMousePress(CtMouseEvent *event)
{
    ctreal x = event->x();
    ctreal y = event->y();

    checkSortedItems();

    foreach_reverse(CtSprite *item, sortedItems) {
        if (!item->isVisible())
            continue;

#ifdef CT_SIMULATE_TOUCH
        if (!(item->flags() & CtSprite::AcceptsTouchEvent) &&
            !(item->flags() & CtSprite::AcceptsMouseEvent))
            continue;
#else
        if (!(item->flags() & CtSprite::AcceptsMouseEvent))
            continue;
#endif

        const CtMatrix &matrix = item->transformMatrix();
        const CtPoint &p1 = matrix.map(x, y);
        const CtPoint &p2 = matrix.map(event->initialX(), event->initialY());

        if (item->contains(p1.x(), p1.y())) {
#ifdef CT_SIMULATE_TOUCH
            CtTouchPoint p(1, p1.x(), p1.y(), p2.x(), p2.y(), 1.0, true);
            CtTouchEvent evt(CtEvent::TouchBegin, 1, p);

            if (item->event(&evt)) {
                touchGrabber = item;
                return true;
            }
#endif
            CtMouseEvent ev(event->type(), event->button(),
                            p1.x(), p1.y(), x, y, p2.x(), p2.y());
            if (item->event(&ev)) {
                mouseGrabber = item;
                return true;
            }
        }
    }

    return false;
}

bool CtSceneViewData::deliverMouseMove(CtMouseEvent *event)
{
#ifndef CT_SIMULATE_TOUCH
    CtSprite *grabber = mouseGrabber;
#else
    CtSprite *grabber = touchGrabber ? touchGrabber : mouseGrabber;
#endif

    if (!grabber)
        return false;

    const CtMatrix &m = grabber->transformMatrix();
    const CtPoint &p1 = m.map(event->x(), event->y());
    const CtPoint &p2 = m.map(event->initialX(), event->initialY());

#ifdef CT_SIMULATE_TOUCH
    if (grabber == touchGrabber) {
        CtTouchPoint p(1, p1.x(), p1.y(), p2.x(), p2.y(), 1.0, true);
        CtTouchEvent evt(CtEvent::TouchUpdate, 1, p);
        bool result = grabber->event(&evt);
        updateDragCursor(grabber);
        return result;
    }
#endif

    CtMouseEvent ev(event->type(), event->button(), p1.x(), p1.y(),
                    event->x(), event->y(), p2.x(), p2.y());
    bool result = grabber->event(&ev);
    updateDragCursor(grabber);
    return result;
}

bool CtSceneViewData::deliverMouseRelease(CtMouseEvent *event)
{
#ifndef CT_SIMULATE_TOUCH
    CtSprite *grabber = mouseGrabber;
#else
    CtSprite *grabber = touchGrabber ? touchGrabber : mouseGrabber;
#endif

    if (!grabber)
        return false;

    const CtMatrix &m = grabber->transformMatrix();
    const CtPoint &p1 = m.map(event->x(), event->y());
    const CtPoint &p2 = m.map(event->initialX(), event->initialY());

#ifdef CT_SIMULATE_TOUCH
    if (grabber == touchGrabber) {
        CtTouchPoint p(1, p1.x(), p1.y(), p2.x(), p2.y(), 1.0, false);
        CtTouchEvent evt(CtEvent::TouchEnd, 1, p);
        bool ok = grabber->event(&evt);
        releaseDragCursor(grabber, false);
        touchGrabber = 0;
        return ok;
    }
#endif

    CtMouseEvent ev(event->type(), event->button(), p1.x(), p1.y(),
                    event->x(), event->y(), p2.x(), p2.y());
    bool ok = mouseGrabber->event(&ev);
    releaseDragCursor(grabber, false);
    mouseGrabber = 0;
    return ok;
}

bool CtSceneViewData::deliverTouchBegin(CtTouchEvent *event)
{
    CtTouchPointList points = event->touchPoints();

    if (points.isEmpty())
        return false;

    checkSortedItems();

    foreach_reverse (CtSprite *item, sortedItems) {
        if (!item->isVisible())
            continue;

        if (!(item->flags() & CtSprite::AcceptsTouchEvent))
            continue;

        CtTouchPointList mappedPoints = mapTouchPoints(item, points);

        foreach (const CtTouchPoint &p, mappedPoints) {
            if (item->contains(p.x(), p.y())) {
                CtTouchEvent ev(event->type(), event->id(), mappedPoints);

                if (item->event(&ev)) {
                    touchGrabber = item;
                    return true;
                }

                break;
            }
        }
    }

    return false;
}

bool CtSceneViewData::deliverTouchUpdate(CtTouchEvent *event)
{
    if (!touchGrabber)
        return false;

    CtTouchEvent ev(event->type(), event->id(),
                    mapTouchPoints(touchGrabber, event->touchPoints()));
    bool result = touchGrabber->event(&ev);
    updateDragCursor(touchGrabber);
    return result;
}

bool CtSceneViewData::deliverTouchEnd(CtTouchEvent *event)
{
    if (!touchGrabber)
        return false;

    CtTouchEvent ev(event->type(), event->id(),
                    mapTouchPoints(touchGrabber, event->touchPoints()));
    bool ok = touchGrabber->event(&ev);
    releaseDragCursor(touchGrabber, false);
    touchGrabber = 0;

    return ok;
}

CtTouchPointList CtSceneViewData::mapTouchPoints(CtSprite *item, const CtTouchPointList &points) const
{
    CtTouchPointList result;
    CtMatrix matrix = item->transformMatrix();

    foreach (const CtTouchPoint &p, points) {
        const CtPoint &p1 = matrix.map(p.x(), p.y());
        const CtPoint &p2 = matrix.map(p.initialX(), p.initialY());

        result.append(CtTouchPoint(p.id(), p1.x(), p1.y(), p2.x(), p2.y(), p.pressure(), p.isPressed()));
    }

    return result;
}


/******************************************************************************
 * CtSceneView
 *****************************************************************************/

CtSceneView::CtSceneView(const char *title, int width, int height)
    : CtWindow(title, width, height)
{
    CT_D(CtWindow);
    d->userData = new CtSceneViewData();
}

CtSceneView::~CtSceneView()
{
    CT_D(CtWindow);
    CtSceneViewData *data = static_cast<CtSceneViewData *>(d->userData);
    delete data;
    d->userData = 0;
}

void CtSceneView::setRootItem(CtSprite *item)
{
    CT_D(CtWindow);
    CtSceneViewData *data = static_cast<CtSceneViewData *>(d->userData);

    if (data->rootItem == item)
        return;

    if (data->rootItem)
        data->rootItem->d_ptr->setScene(0);

    data->rootItem = item;

    if (data->rootItem)
        data->rootItem->d_ptr->setScene(this);

    data->sortedItemsDirty = true;
}

void CtSceneView::advance(ctuint ms)
{
    CT_D(CtWindow);
    CtSceneViewData *data = static_cast<CtSceneViewData *>(d->userData);

    data->checkSortedItems();

    foreach (CtSprite *item, data->sortedItems) {
        if (!item->isFrozen())
            item->advance(ms);
    }

    foreach (CtSprite *item, data->sortedItems) {
        if (item->d_ptr->pendingDelete)
            delete item;
    }
}

void CtSceneView::paint()
{
    CT_D(CtWindow);

    if (isMinimized())
        return;

    CtSceneViewData *data = static_cast<CtSceneViewData *>(d->userData);

    // update if list necessary

    CtRenderer renderer;

    if (data->rootItem && data->rootItem->isVisible())
        data->rootItem->d_ptr->recursivePaint(&renderer);

    CT_GL_DEBUG_CHECK();
}

bool CtSceneView::setDragCursor(CtDragCursor *drag)
{
    CT_D(CtWindow);
    CtSceneViewData *data = static_cast<CtSceneViewData *>(d->userData);

    if (data->drag)
        return false;

    if (drag->sourceItem() != data->mouseGrabber &&
        drag->sourceItem() != data->touchGrabber)
        return false;

    data->drag = drag;
    return true;
}

bool CtSceneView::event(CtEvent *event)
{
    CT_D(CtWindow);
    CtSceneViewData *data = static_cast<CtSceneViewData *>(d->userData);

    bool ok = false;

    switch (event->type()) {
    case CtEvent::MousePress:
        ok = data->deliverMousePress(static_cast<CtMouseEvent *>(event));
        break;
    case CtEvent::MouseMove:
        ok = data->deliverMouseMove(static_cast<CtMouseEvent *>(event));
        break;
    case CtEvent::MouseRelease:
        ok = data->deliverMouseRelease(static_cast<CtMouseEvent *>(event));
        break;
    case CtEvent::TouchBegin:
        ok = data->deliverTouchBegin(static_cast<CtTouchEvent *>(event));
        break;
    case CtEvent::TouchUpdate:
        ok = data->deliverTouchUpdate(static_cast<CtTouchEvent *>(event));
        break;
    case CtEvent::TouchEnd:
        ok = data->deliverTouchEnd(static_cast<CtTouchEvent *>(event));
        break;
    default:
        break;
    };

    if (ok)
        return true;
    else
        return CtWindow::event(event);
}

void CtSceneView::itemAddedToScene(CtSprite *)
{
    CT_D(CtWindow);
    CtSceneViewData *data = static_cast<CtSceneViewData *>(d->userData);
    data->sortedItemsDirty = true;
}

void CtSceneView::itemRemovedFromScene(CtSprite *item)
{
    CT_D(CtWindow);
    CtSceneViewData *data = static_cast<CtSceneViewData *>(d->userData);
    data->sortedItemsDirty = true;

    if (item == data->touchGrabber) {
        data->releaseDragCursor(item, true);
        data->touchGrabber = 0;
    }

    if (item == data->mouseGrabber) {
        data->releaseDragCursor(item, true);
        data->mouseGrabber = 0;
    }
}

void CtSceneView::itemZValueChanged(CtSprite *item)
{
    CT_D(CtWindow);
    CtSceneViewData *data = static_cast<CtSceneViewData *>(d->userData);
    data->sortedItemsDirty = true;
}
