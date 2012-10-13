#include "flsceneview.h"
#include "flitem.h"
#include "flitem_p.h"
#include "fldragcursor.h"
#include "flwindow_p.h"
#include "flopenglfunctions.h"

/******************************************************************************
 * FlSceneViewData
 *****************************************************************************/

struct FlSceneViewData
{
    FlSceneViewData();

    void checkSortedItems();

    void updateDragCursor(FlSceneItem *source);
    void releaseDragCursor(FlSceneItem *source, bool canceled);

    bool deliverMousePress(FlMouseEvent *event);
    bool deliverMouseMove(FlMouseEvent *event);
    bool deliverMouseRelease(FlMouseEvent *event);

    bool deliverTouchBegin(FlTouchEvent *event);
    bool deliverTouchUpdate(FlTouchEvent *event);
    bool deliverTouchEnd(FlTouchEvent *event);

    FlTouchPointList mapTouchPoints(FlSceneItem *item, const FlTouchPointList &points) const;

    static bool sort_compare_zvalue(FlSceneItem *a, FlSceneItem *b) {
        return (!a || !b) ? false : (a->z() > b->z());
    }

    FlDragCursor *drag;
    FlSceneItem *rootItem;
    FlList<FlSceneItem *> sortedItems;
    bool sortedItemsDirty;
    FlSceneItem *touchGrabber;
    FlSceneItem *mouseGrabber;
    FlSceneItem *dragGrabber;
};

FlSceneViewData::FlSceneViewData()
    : drag(0),
      rootItem(0),
      sortedItemsDirty(true),
      touchGrabber(0),
      mouseGrabber(0),
      dragGrabber(0)
{

}

void FlSceneViewData::checkSortedItems()
{
    if (!sortedItemsDirty)
        return;

    sortedItems.clear();

    if (!rootItem)
        return;

    FlList<FlSceneItem *> stack;
    FlList<FlSceneItem *> children;
    stack.push_back(rootItem);

    while (!stack.empty()) {
        FlSceneItem *item = stack.back();
        stack.pop_back();

        sortedItems.push_back(item);

        children = item->children();
        // sort from greater z-value to lower z-value
        children.sort(FlSceneViewData::sort_compare_zvalue);

        foreach (FlSceneItem *item, children)
            stack.push_back(item);
    }

    sortedItemsDirty = false;
}

void FlSceneViewData::updateDragCursor(FlSceneItem *source)
{
    if (!drag || !drag->sourceItem() || drag->sourceItem() != source)
        return;

    const flreal x = drag->x();
    const flreal y = drag->y();
    const FlDragDropEvent::Operation op = (FlDragDropEvent::Operation)drag->operation();

    checkSortedItems();

    FlSceneItem *currentGrabber = 0;

    foreach_reverse(FlSceneItem *item, sortedItems) {
        if (!item->isVisible() || !(item->flags() & FlSceneItem::AcceptsDragEvent))
            continue;

        flreal mx, my;
        const FlMatrix &matrix = item->transformMatrix();
        matrix.map(x, y, &mx, &my);

        if (!item->contains(mx, my))
            continue;

        bool isNew = (dragGrabber != item);
        FlDragDropEvent ev(isNew ? FlEvent::DragEnter : FlEvent::DragMove,
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
        flreal mx, my;
        const FlMatrix &matrix = dragGrabber->transformMatrix();
        matrix.map(x, y, &mx, &my);

        FlDragDropEvent ev(FlEvent::DragLeave, drag->sourceItem(), dragGrabber,
                           drag->draggedItem(), drag->mime(), op, mx, my, x, y);
        dragGrabber->event(&ev);
    }

    dragGrabber = currentGrabber;
}

void FlSceneViewData::releaseDragCursor(FlSceneItem *source, bool canceled)
{
    if (!drag || !drag->sourceItem() || drag->sourceItem() != source)
        return;

    flreal mx = 0, my = 0;
    const flreal x = drag->x();
    const flreal y = drag->y();
    const FlDragDropEvent::Operation op = (FlDragDropEvent::Operation)(drag->operation());

    if (dragGrabber)
        dragGrabber->transformMatrix().map(x, y, &mx, &my);

    if (!dragGrabber || canceled) {
        if (dragGrabber) {
            FlDragDropEvent ev(FlEvent::DragLeave, drag->sourceItem(), dragGrabber,
                               drag->draggedItem(), drag->mime(), op, mx, my, x, y);
            dragGrabber->event(&ev);
        }

        FlDragDropEvent evc(FlEvent::DragCursorCancel, drag->sourceItem(), dragGrabber,
                            drag->draggedItem(), drag->mime(), op, mx, my, x, y);
        drag->sourceItem()->event(&evc);
    } else {
        FlDragDropEvent ev(FlEvent::Drop, drag->sourceItem(), dragGrabber,
                           drag->draggedItem(), drag->mime(), op, mx, my, x, y);
        bool ok = dragGrabber->event(&ev);

        FlDragDropEvent evc(ok ? FlEvent::DragCursorDrop : FlEvent::DragCursorCancel,
                            drag->sourceItem(), dragGrabber, drag->draggedItem(),
                            drag->mime(), op, mx, my, x, y);
        drag->sourceItem()->event(&evc);
    }

    drag = 0;
    dragGrabber = 0;
}

bool FlSceneViewData::deliverMousePress(FlMouseEvent *event)
{
    flreal mx, my, ix, iy;
    flreal x = event->x();
    flreal y = event->y();

    checkSortedItems();

    foreach_reverse(FlSceneItem *item, sortedItems) {
        if (!item->isVisible())
            continue;

#ifdef FL_SIMULATE_TOUCH
        if (!(item->flags() & FlSceneItem::AcceptsTouchEvent) &&
            !(item->flags() & FlSceneItem::AcceptsMouseEvent))
            continue;
#else
        if (!(item->flags() & FlSceneItem::AcceptsMouseEvent))
            continue;
#endif

        const FlMatrix &matrix = item->transformMatrix();
        matrix.map(x, y, &mx, &my);
        matrix.map(event->initialX(), event->initialY(), &ix, &iy);

        if (item->contains(mx, my)) {
#ifdef FL_SIMULATE_TOUCH
            FlTouchPoint p(1, mx, my, ix, iy, 1.0, true);
            FlTouchEvent evt(FlEvent::TouchBegin, 1, p);

            if (item->event(&evt)) {
                touchGrabber = item;
                return true;
            }
#endif
            FlMouseEvent ev(event->type(), event->button(),
                            mx, my, x, y, ix, iy);
            if (item->event(&ev)) {
                mouseGrabber = item;
                return true;
            }
        }
    }

    return false;
}

bool FlSceneViewData::deliverMouseMove(FlMouseEvent *event)
{
#ifndef FL_SIMULATE_TOUCH
    FlSceneItem *grabber = mouseGrabber;
#else
    FlSceneItem *grabber = touchGrabber ? touchGrabber : mouseGrabber;
#endif

    if (!grabber)
        return false;

    flreal mx, my, ix, iy;

    const FlMatrix &m = grabber->transformMatrix();
    m.map(event->x(), event->y(), &mx, &my);
    m.map(event->initialX(), event->initialY(), &ix, &iy);

#ifdef FL_SIMULATE_TOUCH
    if (grabber == touchGrabber) {
        FlTouchPoint p(1, mx, my, ix, iy, 1.0, true);
        FlTouchEvent evt(FlEvent::TouchUpdate, 1, p);
        bool result = grabber->event(&evt);
        updateDragCursor(grabber);
        return result;
    }
#endif

    FlMouseEvent ev(event->type(), event->button(), mx, my,
                    event->x(), event->y(), ix, iy);
    bool result = grabber->event(&ev);
    updateDragCursor(grabber);
    return result;
}

bool FlSceneViewData::deliverMouseRelease(FlMouseEvent *event)
{
#ifndef FL_SIMULATE_TOUCH
    FlSceneItem *grabber = mouseGrabber;
#else
    FlSceneItem *grabber = touchGrabber ? touchGrabber : mouseGrabber;
#endif

    if (!grabber)
        return false;

    flreal mx, my, ix, iy;

    const FlMatrix &m = grabber->transformMatrix();
    m.map(event->x(), event->y(), &mx, &my);
    m.map(event->initialX(), event->initialY(), &ix, &iy);

#ifdef FL_SIMULATE_TOUCH
    if (grabber == touchGrabber) {
        FlTouchPoint p(1, mx, my, ix, iy, 1.0, false);
        FlTouchEvent evt(FlEvent::TouchEnd, 1, p);
        bool ok = grabber->event(&evt);
        releaseDragCursor(grabber, false);
        touchGrabber = 0;
        return ok;
    }
#endif

    FlMouseEvent ev(event->type(), event->button(), mx, my,
                    event->x(), event->y(), ix, iy);
    bool ok = mouseGrabber->event(&ev);
    releaseDragCursor(grabber, false);
    mouseGrabber = 0;
    return ok;
}

bool FlSceneViewData::deliverTouchBegin(FlTouchEvent *event)
{
    FlTouchPointList points = event->touchPoints();

    if (points.empty())
        return false;

    checkSortedItems();

    foreach_reverse (FlSceneItem *item, sortedItems) {
        if (!item->isVisible())
            continue;

        if (!(item->flags() & FlSceneItem::AcceptsTouchEvent))
            continue;

        FlTouchPointList mappedPoints = mapTouchPoints(item, points);

        foreach (const FlTouchPoint &p, mappedPoints) {
            if (item->contains(p.x(), p.y())) {
                FlTouchEvent ev(event->type(), event->id(), mappedPoints);

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

bool FlSceneViewData::deliverTouchUpdate(FlTouchEvent *event)
{
    if (!touchGrabber)
        return false;

    FlTouchEvent ev(event->type(), event->id(),
                    mapTouchPoints(touchGrabber, event->touchPoints()));
    bool result = touchGrabber->event(&ev);
    updateDragCursor(touchGrabber);
    return result;
}

bool FlSceneViewData::deliverTouchEnd(FlTouchEvent *event)
{
    if (!touchGrabber)
        return false;

    FlTouchEvent ev(event->type(), event->id(),
                    mapTouchPoints(touchGrabber, event->touchPoints()));
    bool ok = touchGrabber->event(&ev);
    releaseDragCursor(touchGrabber, false);
    touchGrabber = 0;

    return ok;
}

FlTouchPointList FlSceneViewData::mapTouchPoints(FlSceneItem *item, const FlTouchPointList &points) const
{
    FlTouchPointList result;
    FlMatrix matrix = item->transformMatrix();

    foreach (const FlTouchPoint &p, points) {
        flreal mx, my;
        matrix.map(p.x(), p.y(), &mx, &my);

        flreal ix, iy;
        matrix.map(p.initialX(), p.initialY(), &ix, &iy);

        result.push_back(FlTouchPoint(p.id(), mx, my, ix, iy, p.pressure(), p.isPressed()));
    }

    return result;
}


/******************************************************************************
 * FlSceneView
 *****************************************************************************/

FlSceneView::FlSceneView(const char *title, int width, int height)
    : FlWindow(title, width, height)
{
    FL_D(FlWindow);
    d->userData = new FlSceneViewData();
}

FlSceneView::~FlSceneView()
{
    FL_D(FlWindow);
    FlSceneViewData *data = static_cast<FlSceneViewData *>(d->userData);
    delete data;
    d->userData = 0;
}

void FlSceneView::setRootItem(FlSceneItem *item)
{
    FL_D(FlWindow);
    FlSceneViewData *data = static_cast<FlSceneViewData *>(d->userData);

    if (data->rootItem == item)
        return;

    if (data->rootItem)
        data->rootItem->d_ptr->setScene(0);

    data->rootItem = item;

    if (data->rootItem)
        data->rootItem->d_ptr->setScene(this);

    data->sortedItemsDirty = true;
}

void FlSceneView::advance(fluint ms)
{
    FL_D(FlWindow);
    FlSceneViewData *data = static_cast<FlSceneViewData *>(d->userData);

    data->checkSortedItems();

    foreach (FlSceneItem *item, data->sortedItems)
        item->advance(ms);
}

void FlSceneView::paint()
{
    FL_D(FlWindow);
    FlSceneViewData *data = static_cast<FlSceneViewData *>(d->userData);

    FlGL::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    FlGL::glEnable(GL_BLEND);

    FlGL::glViewport(0, 0, width(), height());

    FlGL::glClearColor(1.0, 1.0, 1.0, 1.0);
    FlGL::glClear(GL_COLOR_BUFFER_BIT);

    // update if list necessary
    data->checkSortedItems();

    foreach (FlSceneItem *item, data->sortedItems) {
        if (item->isVisible())
            item->paint();
    }

    FL_GL_DEBUG_CHECK();

    swapBuffers();
}

bool FlSceneView::setDragCursor(FlDragCursor *drag)
{
    FL_D(FlWindow);
    FlSceneViewData *data = static_cast<FlSceneViewData *>(d->userData);

    if (data->drag)
        return false;

    if (drag->sourceItem() != data->mouseGrabber &&
        drag->sourceItem() != data->touchGrabber)
        return false;

    data->drag = drag;
    return true;
}

bool FlSceneView::event(FlEvent *event)
{
    FL_D(FlWindow);
    FlSceneViewData *data = static_cast<FlSceneViewData *>(d->userData);

    bool ok = false;

    switch (event->type()) {
    case FlEvent::MousePress:
        ok = data->deliverMousePress(static_cast<FlMouseEvent *>(event));
        break;
    case FlEvent::MouseMove:
        ok = data->deliverMouseMove(static_cast<FlMouseEvent *>(event));
        break;
    case FlEvent::MouseRelease:
        ok = data->deliverMouseRelease(static_cast<FlMouseEvent *>(event));
        break;
    case FlEvent::TouchBegin:
        ok = data->deliverTouchBegin(static_cast<FlTouchEvent *>(event));
        break;
    case FlEvent::TouchUpdate:
        ok = data->deliverTouchUpdate(static_cast<FlTouchEvent *>(event));
        break;
    case FlEvent::TouchEnd:
        ok = data->deliverTouchEnd(static_cast<FlTouchEvent *>(event));
        break;
    default:
        break;
    };

    if (ok)
        return true;
    else
        return FlWindow::event(event);
}

void FlSceneView::itemAddedToScene(FlSceneItem *)
{
    FL_D(FlWindow);
    FlSceneViewData *data = static_cast<FlSceneViewData *>(d->userData);
    data->sortedItemsDirty = true;
}

void FlSceneView::itemRemovedFromScene(FlSceneItem *item)
{
    FL_D(FlWindow);
    FlSceneViewData *data = static_cast<FlSceneViewData *>(d->userData);
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

void FlSceneView::itemZValueChanged(FlSceneItem *item)
{
    FL_D(FlWindow);
    FlSceneViewData *data = static_cast<FlSceneViewData *>(d->userData);
    data->sortedItemsDirty = true;
}
