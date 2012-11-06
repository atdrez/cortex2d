#include "ctsceneview.h"
#include "ctitem.h"
#include "ctitem_p.h"
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

    void updateDragCursor(CtSceneItem *source);
    void releaseDragCursor(CtSceneItem *source, bool canceled);

    bool deliverMousePress(CtMouseEvent *event);
    bool deliverMouseMove(CtMouseEvent *event);
    bool deliverMouseRelease(CtMouseEvent *event);

    bool deliverTouchBegin(CtTouchEvent *event);
    bool deliverTouchUpdate(CtTouchEvent *event);
    bool deliverTouchEnd(CtTouchEvent *event);

    CtTouchPointList mapTouchPoints(CtSceneItem *item, const CtTouchPointList &points) const;

    static bool sort_compare_zvalue(CtSceneItem *a, CtSceneItem *b) {
        return (!a || !b) ? false : (a->z() > b->z());
    }

    CtDragCursor *drag;
    CtSceneItem *rootItem;
    CtList<CtSceneItem *> sortedItems;
    bool sortedItemsDirty;
    CtSceneItem *touchGrabber;
    CtSceneItem *mouseGrabber;
    CtSceneItem *dragGrabber;
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

    if (!rootItem)
        return;

    CtList<CtSceneItem *> stack;
    CtList<CtSceneItem *> children;
    stack.push_back(rootItem);

    while (!stack.empty()) {
        CtSceneItem *item = stack.back();
        stack.pop_back();

        sortedItems.push_back(item);

        children = item->children();
        // sort from greater z-value to lower z-value
        children.sort(CtSceneViewData::sort_compare_zvalue);

        foreach (CtSceneItem *item, children)
            stack.push_back(item);
    }

    sortedItemsDirty = false;
}

void CtSceneViewData::updateDragCursor(CtSceneItem *source)
{
    if (!drag || !drag->sourceItem() || drag->sourceItem() != source)
        return;

    const ctreal x = drag->x();
    const ctreal y = drag->y();
    const CtDragDropEvent::Operation op = (CtDragDropEvent::Operation)drag->operation();

    checkSortedItems();

    CtSceneItem *currentGrabber = 0;

    foreach_reverse(CtSceneItem *item, sortedItems) {
        if (!item->isVisible() || !(item->flags() & CtSceneItem::AcceptsDragEvent))
            continue;

        ctreal mx, my;
        const CtMatrix &matrix = item->transformMatrix();
        matrix.map(x, y, &mx, &my);

        if (!item->contains(mx, my))
            continue;

        bool isNew = (dragGrabber != item);
        CtDragDropEvent ev(isNew ? CtEvent::DragEnter : CtEvent::DragMove,
                           drag->sourceItem(), item, drag->draggedItem(),
                           drag->mime(), op, mx, my, x, y);

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
        ctreal mx, my;
        const CtMatrix &matrix = dragGrabber->transformMatrix();
        matrix.map(x, y, &mx, &my);

        CtDragDropEvent ev(CtEvent::DragLeave, drag->sourceItem(), dragGrabber,
                           drag->draggedItem(), drag->mime(), op, mx, my, x, y);
        dragGrabber->event(&ev);
    }

    dragGrabber = currentGrabber;
}

void CtSceneViewData::releaseDragCursor(CtSceneItem *source, bool canceled)
{
    if (!drag || !drag->sourceItem() || drag->sourceItem() != source)
        return;

    ctreal mx = 0, my = 0;
    const ctreal x = drag->x();
    const ctreal y = drag->y();
    const CtDragDropEvent::Operation op = (CtDragDropEvent::Operation)(drag->operation());

    if (dragGrabber)
        dragGrabber->transformMatrix().map(x, y, &mx, &my);

    if (!dragGrabber || canceled) {
        if (dragGrabber) {
            CtDragDropEvent ev(CtEvent::DragLeave, drag->sourceItem(), dragGrabber,
                               drag->draggedItem(), drag->mime(), op, mx, my, x, y);
            dragGrabber->event(&ev);
        }

        CtDragDropEvent evc(CtEvent::DragCursorCancel, drag->sourceItem(), dragGrabber,
                            drag->draggedItem(), drag->mime(), op, mx, my, x, y);
        drag->sourceItem()->event(&evc);
    } else {
        CtDragDropEvent ev(CtEvent::Drop, drag->sourceItem(), dragGrabber,
                           drag->draggedItem(), drag->mime(), op, mx, my, x, y);
        bool ok = dragGrabber->event(&ev);

        CtDragDropEvent evc(ok ? CtEvent::DragCursorDrop : CtEvent::DragCursorCancel,
                            drag->sourceItem(), dragGrabber, drag->draggedItem(),
                            drag->mime(), op, mx, my, x, y);
        drag->sourceItem()->event(&evc);
    }

    drag = 0;
    dragGrabber = 0;
}

bool CtSceneViewData::deliverMousePress(CtMouseEvent *event)
{
    ctreal mx, my, ix, iy;
    ctreal x = event->x();
    ctreal y = event->y();

    checkSortedItems();

    foreach_reverse(CtSceneItem *item, sortedItems) {
        if (!item->isVisible())
            continue;

#ifdef CT_SIMULATE_TOUCH
        if (!(item->flags() & CtSceneItem::AcceptsTouchEvent) &&
            !(item->flags() & CtSceneItem::AcceptsMouseEvent))
            continue;
#else
        if (!(item->flags() & CtSceneItem::AcceptsMouseEvent))
            continue;
#endif

        const CtMatrix &matrix = item->transformMatrix();
        matrix.map(x, y, &mx, &my);
        matrix.map(event->initialX(), event->initialY(), &ix, &iy);

        if (item->contains(mx, my)) {
#ifdef CT_SIMULATE_TOUCH
            CtTouchPoint p(1, mx, my, ix, iy, 1.0, true);
            CtTouchEvent evt(CtEvent::TouchBegin, 1, p);

            if (item->event(&evt)) {
                touchGrabber = item;
                return true;
            }
#endif
            CtMouseEvent ev(event->type(), event->button(),
                            mx, my, x, y, ix, iy);
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
    CtSceneItem *grabber = mouseGrabber;
#else
    CtSceneItem *grabber = touchGrabber ? touchGrabber : mouseGrabber;
#endif

    if (!grabber)
        return false;

    ctreal mx, my, ix, iy;

    const CtMatrix &m = grabber->transformMatrix();
    m.map(event->x(), event->y(), &mx, &my);
    m.map(event->initialX(), event->initialY(), &ix, &iy);

#ifdef CT_SIMULATE_TOUCH
    if (grabber == touchGrabber) {
        CtTouchPoint p(1, mx, my, ix, iy, 1.0, true);
        CtTouchEvent evt(CtEvent::TouchUpdate, 1, p);
        bool result = grabber->event(&evt);
        updateDragCursor(grabber);
        return result;
    }
#endif

    CtMouseEvent ev(event->type(), event->button(), mx, my,
                    event->x(), event->y(), ix, iy);
    bool result = grabber->event(&ev);
    updateDragCursor(grabber);
    return result;
}

bool CtSceneViewData::deliverMouseRelease(CtMouseEvent *event)
{
#ifndef CT_SIMULATE_TOUCH
    CtSceneItem *grabber = mouseGrabber;
#else
    CtSceneItem *grabber = touchGrabber ? touchGrabber : mouseGrabber;
#endif

    if (!grabber)
        return false;

    ctreal mx, my, ix, iy;

    const CtMatrix &m = grabber->transformMatrix();
    m.map(event->x(), event->y(), &mx, &my);
    m.map(event->initialX(), event->initialY(), &ix, &iy);

#ifdef CT_SIMULATE_TOUCH
    if (grabber == touchGrabber) {
        CtTouchPoint p(1, mx, my, ix, iy, 1.0, false);
        CtTouchEvent evt(CtEvent::TouchEnd, 1, p);
        bool ok = grabber->event(&evt);
        releaseDragCursor(grabber, false);
        touchGrabber = 0;
        return ok;
    }
#endif

    CtMouseEvent ev(event->type(), event->button(), mx, my,
                    event->x(), event->y(), ix, iy);
    bool ok = mouseGrabber->event(&ev);
    releaseDragCursor(grabber, false);
    mouseGrabber = 0;
    return ok;
}

bool CtSceneViewData::deliverTouchBegin(CtTouchEvent *event)
{
    CtTouchPointList points = event->touchPoints();

    if (points.empty())
        return false;

    checkSortedItems();

    foreach_reverse (CtSceneItem *item, sortedItems) {
        if (!item->isVisible())
            continue;

        if (!(item->flags() & CtSceneItem::AcceptsTouchEvent))
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

CtTouchPointList CtSceneViewData::mapTouchPoints(CtSceneItem *item, const CtTouchPointList &points) const
{
    CtTouchPointList result;
    CtMatrix matrix = item->transformMatrix();

    foreach (const CtTouchPoint &p, points) {
        ctreal mx, my;
        matrix.map(p.x(), p.y(), &mx, &my);

        ctreal ix, iy;
        matrix.map(p.initialX(), p.initialY(), &ix, &iy);

        result.push_back(CtTouchPoint(p.id(), mx, my, ix, iy, p.pressure(), p.isPressed()));
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

void CtSceneView::setRootItem(CtSceneItem *item)
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

    foreach (CtSceneItem *item, data->sortedItems)
        item->advance(ms);

    foreach (CtSceneItem *item, data->sortedItems) {
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

    CtGL::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CtGL::glEnable(GL_BLEND);

    CtGL::glViewport(0, 0, width(), height());

    CtGL::glClearColor(1.0, 1.0, 1.0, 1.0);
    CtGL::glClear(GL_COLOR_BUFFER_BIT);

    // update if list necessary
    data->checkSortedItems();

    foreach (CtSceneItem *item, data->sortedItems) {
        if (item->isVisible())
            item->paint();
    }

    CT_GL_DEBUG_CHECK();

    swapBuffers();
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

void CtSceneView::itemAddedToScene(CtSceneItem *)
{
    CT_D(CtWindow);
    CtSceneViewData *data = static_cast<CtSceneViewData *>(d->userData);
    data->sortedItemsDirty = true;
}

void CtSceneView::itemRemovedFromScene(CtSceneItem *item)
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

void CtSceneView::itemZValueChanged(CtSceneItem *item)
{
    CT_D(CtWindow);
    CtSceneViewData *data = static_cast<CtSceneViewData *>(d->userData);
    data->sortedItemsDirty = true;
}
