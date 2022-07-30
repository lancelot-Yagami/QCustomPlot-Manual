#include "qcpsizehandle.h"

QCPSizeHandle::QCPSizeHandle(QCustomPlot *parentPlot, QCPAbstractItem *attachItem)
    : QCPAbstractItem(parentPlot)
    , position(createPosition(QLatin1String("position")))
    , mHovered(false)
    , mAttachItem(attachItem)
{
    setBrush(QColor("#436EEE"));
    setHoveredBrush(QColor("#1C86EE"));
    setSelectedBrush(QColor("#3A5FCD"));
    setSize(8);
    setLayer(QLatin1String("overlay"));
}

QCPSizeHandle::~QCPSizeHandle()
{
}

void QCPSizeHandle::setBrush(const QBrush &brush)
{
    mBrush = brush;
}

void QCPSizeHandle::setSelectedBrush(const QBrush &brush)
{
    mSelectedBrush = brush;
}

void QCPSizeHandle::setHoveredBrush(const QBrush &brush)
{
    mHoveredBrush = brush;
}

void QCPSizeHandle::setSize(double size)
{
    mSize = size;
}

double QCPSizeHandle::selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const
{
    Q_UNUSED(details)
    if (onlySelectable && !mSelectable)
        return -1;

    QPointF itemPos = position->pixelPosition();
    QRectF rect = QRectF(itemPos.x() - mSize * 0.5, itemPos.y() - mSize * 0.5, mSize, mSize);
    bool filledRect = mBrush.style() != Qt::NoBrush && mBrush.color().alpha() != 0;
    return rectDistance(rect, pos, filledRect);
}

void QCPSizeHandle::draw(QCPPainter *painter)
{
    QRectF rect(0, 0, mSize, mSize);

    rect.moveTopLeft(position->pixelPosition() - QPointF(mSize * 0.5, mSize * 0.5));

    if (rect.intersects(clipRect())) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(mainBrush());
        painter->drawRect(rect);
    }
}

//void QCPSizeHandle::hoverEnterEvent(QHoverEvent *event)
//{
//    Q_UNUSED(event)
//    mHovered = true;
//    layer()->replot();
//}

//void QCPSizeHandle::hoverLeaveEvent(QHoverEvent *event)
//{
//    Q_UNUSED(event)
//    mHovered = true;
//    layer()->replot();
//}

QBrush QCPSizeHandle::mainBrush() const
{
    return selected() ? mSelectedBrush : (mHovered ? mHoveredBrush : mBrush);
}
