#include "qcptextbars.h"

QCPTextBars::QCPTextBars(QCPAxis *keyAxis, QCPAxis *valueAxis)
    : QCPBars(keyAxis, valueAxis)
    , mTextAlignment(Qt::AlignCenter)
    , mTextVisible(false)
    , mSpacing(5)
    , mFont(QFont(QLatin1String("sans serif"), 12))
{

}

void QCPTextBars::setTextAlignment(Qt::Alignment alignment)
{
    mTextAlignment = alignment;
}

void QCPTextBars::setTextVisible(bool visible)
{
    mTextVisible = visible;
}

void QCPTextBars::setSpacing(double spacing)
{
    mSpacing = spacing;
}

void QCPTextBars::setFont(const QFont &font)
{
    mFont = font;
}

void QCPTextBars::draw(QCPPainter *painter)
{
    if (!mKeyAxis || !mValueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
    if (mDataContainer->isEmpty()) return;

    QCPBarsDataContainer::const_iterator visibleBegin, visibleEnd;
    getVisibleDataBounds(visibleBegin, visibleEnd);

    // loop over and draw segments of unselected/selected data:
    QList<QCPDataRange> selectedSegments, unselectedSegments, allSegments;
    getDataSegments(selectedSegments, unselectedSegments);
    allSegments << unselectedSegments << selectedSegments;
    for (int i=0; i<allSegments.size(); ++i)
    {
        bool isSelectedSegment = i >= unselectedSegments.size();
        QCPBarsDataContainer::const_iterator begin = visibleBegin;
        QCPBarsDataContainer::const_iterator end = visibleEnd;
        mDataContainer->limitIteratorsToDataRange(begin, end, allSegments.at(i));
        if (begin == end)
            continue;

        for (QCPBarsDataContainer::const_iterator it=begin; it!=end; ++it)
        {
            // check data validity if flag set:
#ifdef QCUSTOMPLOT_CHECK_DATA
            if (QCP::isInvalidData(it->key, it->value))
                qDebug() << Q_FUNC_INFO << "Data point at" << it->key << "of drawn range invalid." << "Plottable name:" << name();
#endif
            // draw bar:
            if (isSelectedSegment && mSelectionDecorator)
            {
                mSelectionDecorator->applyBrush(painter);
                mSelectionDecorator->applyPen(painter);
            } else
            {
                painter->setBrush(mBrush);
                painter->setPen(mPen);
            }
            applyDefaultAntialiasingHint(painter);

            QRectF barRect = getBarRect(it->key, it->value);
            painter->drawPolygon(barRect);

            if (mTextVisible)
                drawText(painter, it->key, it->value, barRect);
        }
    }

    // draw other selection decoration that isn't just line/scatter pens and brushes:
    if (mSelectionDecorator)
        mSelectionDecorator->drawDecoration(painter, selection());
}

void QCPTextBars::drawText(QCPPainter *painter, double key, double value, const QRectF &barRect)
{
    Q_UNUSED(key)

    painter->setFont(mFont);
    QString text = QString::number(value, 'g', 2);   // 取得当前value轴的值，保留两位精度

    QRectF textRect = painter->fontMetrics().boundingRect(0, 0, 0, 0, Qt::TextDontClip | mTextAlignment, text);  // 计算文字所占用的大小

    if (mKeyAxis.data()->orientation() == Qt::Horizontal) {    // 当key轴为水平轴的时候
        if (mKeyAxis.data()->axisType() == QCPAxis::atTop)     // 上轴，移动文字到柱状图下面
            textRect.moveTopLeft(barRect.bottomLeft() + QPointF(0, mSpacing));
        else                                                   // 下轴，移动文字到柱状图上面
            textRect.moveBottomLeft(barRect.topLeft() - QPointF(0, mSpacing));
        textRect.setWidth(barRect.width());
        painter->drawText(textRect, Qt::TextDontClip | mTextAlignment, text);
    } else {                                                  // 当key轴为竖直轴的时候
        if (mKeyAxis.data()->axisType() == QCPAxis::atLeft)   // 左轴，移动文字到柱状图右边
            textRect.moveTopLeft(barRect.topRight() + QPointF(mSpacing, 0));
        else                                                  // 右轴，移动文字到柱状图左边
            textRect.moveTopRight(barRect.topLeft() - QPointF(mSpacing, 0));
        textRect.setHeight(barRect.height());
        painter->drawText(textRect, Qt::TextDontClip | mTextAlignment, text);
    }
}
