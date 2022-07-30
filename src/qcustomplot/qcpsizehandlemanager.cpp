#include "qcpsizehandlemanager.h"

QCPSizeHandleManager::QCPSizeHandleManager(QCustomPlot *parent)
    : QCPAbstractItem(parent)
{
    setLayer("overlay");
}

bool QCPSizeHandleManager::addItem(QCPAbstractItem *item)
{
    if (!item || item->positions().isEmpty() || qobject_cast<QCPSizeHandle*>(item))
        return false;

    if (mItems.contains(item))
        return true;

    QList<QCPSizeHandle*> handles;
    for (auto position : item->positions()) {
        auto *handle = new QCPSizeHandle(mParentPlot, item);
        handle->position->setParentAnchor(position);
        handles.append(handle);
    }

    if (handles.count() == 1)
        handles.at(0)->setVisible(false);

    auto itemData = QSharedPointer<ItemData>::create();
    itemData->resizable = handles.count() > 1;
    itemData->handles = handles;
    mItems.insert(item, itemData);
    return true;
}

void QCPSizeHandleManager::removeItem(QCPAbstractItem *item)
{
    mItems.remove(item);
}

void QCPSizeHandleManager::setLineVisible(QCPAbstractItem *item, bool visible)
{
    if (auto itemData = mItems.value(item))
        itemData->lineVisible = visible;
}

void QCPSizeHandleManager::setLinePen(QCPAbstractItem *item, const QPen &pen)
{
    if (auto itemData = mItems.value(item))
        itemData->linePen = pen;
}

void QCPSizeHandleManager::setItemMovable(QCPAbstractItem *item, bool movable)
{
    if (auto itemData = mItems.value(item))
        itemData->movable = movable;
}

void QCPSizeHandleManager::setItemResizable(QCPAbstractItem *item, bool resizable)
{
    if (auto itemData = mItems.value(item))
        itemData->resizable = resizable && itemData->handles.count() > 1;
}

double QCPSizeHandleManager::selectTest(const QPointF &, bool , QVariant *) const
{
    return -1;
}

void QCPSizeHandleManager::onItemMove(QCPAbstractItem *item, const QPointF &delta)
{
    if (!item || !mItems.contains(item))
        return;

    auto itemData = mItems.value(item);
    if (!itemData->movable)
        return;

    for (auto position : item->positions())
        position->setPixelPosition(position->pixelPosition() + delta);
}

void QCPSizeHandleManager::onItemResize(QCPSizeHandle *sizeHandle, const QPointF &delta)
{
    if (!sizeHandle)
        return;

    auto itemData = mItems.value(sizeHandle->attachItem());
    if (!itemData || !itemData->resizable)
        return;

    if (auto parentPosition = static_cast<QCPItemPosition*>(sizeHandle->position->parentAnchor()))
    {
        parentPosition->setPixelPosition(parentPosition->pixelPosition() + delta);
    }
}

void QCPSizeHandleManager::draw(QCPPainter *painter)
{
    QMapIterator<QPointer<QCPAbstractItem>, QSharedPointer<ItemData>> i(mItems);
    QList<QCPAbstractItem*> destoryedItems;

    while (i.hasNext()) {
        i.next();

        if (!i.key()) {
            destoryedItems.append(i.key());
            continue;
        }

        auto itemData = i.value();
        if (!itemData->lineVisible || itemData->handles.count() < 2)
            continue;

        painter->setPen(itemData->linePen);

        QVector<QPointF> lines;

        QCPItemAnchor *topLeft = nullptr;
        QCPItemAnchor *bottomRight = nullptr;

        for (auto handle : itemData->handles) {
            if (handle->position->parentAnchor()->name() == QLatin1String("topLeft"))
                topLeft = handle->position->parentAnchor();
            if (handle->position->parentAnchor()->name() == QLatin1String("bottomRight"))
                bottomRight = handle->position->parentAnchor();
            lines.push_back(handle->position->pixelPosition());
        }

        if (topLeft && bottomRight) {
            lines.clear();

            lines.push_back(topLeft->pixelPosition());
            lines.push_back(QPointF(bottomRight->pixelPosition().x(), topLeft->pixelPosition().y()));
            lines.push_back(bottomRight->pixelPosition());
            lines.push_back(QPointF(topLeft->pixelPosition().x(), bottomRight->pixelPosition().y()));
            lines.push_back(topLeft->pixelPosition());
            painter->drawPolyline(lines);
        } else
            painter->drawLines(lines);
    }
}
