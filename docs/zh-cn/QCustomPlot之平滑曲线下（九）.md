接上篇[QCustomPlot之平滑曲线上（八）](zh-cn/QCustomPlot之平滑曲线上（八）)，上篇只是实现了平滑曲线的绘制，但是并没有实现平滑曲线与0点线之间的填充区域以及两个`QCPGraph`之间的填充区域，我们将在这里实现它们

#### drawFill函数的修改
```
void QCPSmoothCurve::drawFill(QCPPainter *painter, QVector<QPointF> *lines) const
{
    if (mLineStyle == lsImpulse) return; // fill doesn't make sense for impulse plot
    if (painter->brush().style() == Qt::NoBrush || painter->brush().color().alpha() == 0) return;

    applyFillAntialiasingHint(painter);
    QVector<QCPDataRange> segments = getNonNanSegments(lines, keyAxis()->orientation());
    if (!mChannelFillGraph)  // 与0点线围成的区域
    {
        // draw base fill under graph, fill goes all the way to the zero-value-line:
        for (int i=0; i<segments.size(); ++i)
            if (mSmooth && mLineStyle == lsLine)
                painter->drawPath(getSmoothFillPath(lines, segments.at(i)));   // 平滑曲线
            else
                painter->drawPolygon(getFillPolygon(lines, segments.at(i)));   // 折线
    } else   // 与其它QCPGraph围成的区域
    {
        // draw fill between this graph and mChannelFillGraph:
        auto channelFillGraph = qobject_cast<QCPSmoothCurve*>(mChannelFillGraph.data());
        if (!channelFillGraph) {
            qDebug() << "ChannelFillGraph must be QCPSmoothCurve";
            return;
        }
        QVector<QPointF> otherLines;
        channelFillGraph->getLines(&otherLines, QCPDataRange(0, channelFillGraph->dataCount()));
        if (!otherLines.isEmpty())
        {
            QVector<QCPDataRange> otherSegments = getNonNanSegments(&otherLines, channelFillGraph->keyAxis()->orientation());
            QVector<QPair<QCPDataRange, QCPDataRange> > segmentPairs = getOverlappingSegments(segments, lines, otherSegments, &otherLines);
            for (int i=0; i<segmentPairs.size(); ++i) {
                if (mSmooth && mLineStyle == lsLine && channelFillGraph->mLineStyle == lsLine)
                    painter->drawPath(getSmoothChannelFillPath(lines, segmentPairs.at(i).first, &otherLines, segmentPairs.at(i).second));  // 平滑曲线
                else
                    painter->drawPolygon(getChannelFillPolygon(lines, segmentPairs.at(i).first, &otherLines, segmentPairs.at(i).second));   // 折线
            }
        }
    }
}
```

#### 与0点线之间的填充区域

```
QPainterPath QCPSmoothCurve::getSmoothFillPath(const QVector<QPointF> *lineData, QCPDataRange segment) const
{
    // 只有一个点构不成填充区域
    if (segment.size() < 2)
        return QPainterPath();

    // 起点，终点对应在轴上的位置
    QPointF start = getFillBasePoint(lineData->at(segment.begin()));
    QPointF end = getFillBasePoint(lineData->at(segment.end() - 1));

    // 将平滑曲线连成一个封闭区域
    QPainterPath path = SmoothCurveGenerator::generateSmoothCurve(*lineData);
    path.lineTo(end);
    path.lineTo(start);
    path.lineTo(lineData->at(segment.begin()));
    return path;
}
```

![与0点线之间的填充区域](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC02Y2ZkNDM4NDUxNGQwOGM4LnBuZw?x-oss-process=image/format,png)


#### 与其它QCPGraph围成的区域
`getSmoothChannelFillPath`基本是从`getChannelFillPolygon`复制过来的，我们在这上面进行修改，修改的内容我都有注释
```
QPainterPath QCPSmoothCurve::getSmoothChannelFillPath(const QVector<QPointF> *thisData, QCPDataRange thisSegment,
                                                      const QVector<QPointF> *otherData, QCPDataRange otherSegment) const
{
    QPainterPath result;
    auto channelFillGraph = qobject_cast<QCPSmoothCurve*>(mChannelFillGraph.data());
    if (!channelFillGraph) {
        qDebug() << "ChannelFillGraph must be QCPSmoothCurve";
        return result;
    }

    QCPAxis *keyAxis = mKeyAxis.data();
    QCPAxis *valueAxis = mValueAxis.data();
    if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return result; }
    if (!channelFillGraph->mKeyAxis) { qDebug() << Q_FUNC_INFO << "channel fill target key axis invalid"; return result; }

    if (channelFillGraph->mKeyAxis.data()->orientation() != keyAxis->orientation())
        return result; // don't have same axis orientation, can't fill that (Note: if keyAxis fits, valueAxis will fit too, because it's always orthogonal to keyAxis)

    if (thisData->isEmpty()) return result;
    QVector<QPointF> thisSegmentData(thisSegment.size());
    QVector<QPointF> otherSegmentData(otherSegment.size());
    std::copy(thisData->constBegin()+thisSegment.begin(), thisData->constBegin()+thisSegment.end(), thisSegmentData.begin());
    std::copy(otherData->constBegin()+otherSegment.begin(), otherData->constBegin()+otherSegment.end(), otherSegmentData.begin());
    // pointers to be able to swap them, depending which data range needs cropping:
    QVector<QPointF> *staticData = &thisSegmentData;
    QVector<QPointF> *croppedData = &otherSegmentData;

    //! [1] 以下为添加的内容
    result = SmoothCurveGenerator::generateSmoothCurve(thisSegmentData);
    if (channelFillGraph->mSmooth && channelFillGraph->mLineStyle == lsLine) {  // mChannelFillGraph也是平滑曲线
        QVector<QPointF> otherSegmentDataReverse(otherSegmentData.size());
        for (int i = otherSegmentData.size() - 1; i >= 0; --i)
            otherSegmentDataReverse[otherSegmentData.size() - i - 1] = otherSegmentData.at(i);
        result = SmoothCurveGenerator::generateSmoothCurve(result, otherSegmentDataReverse);
    } else {  // mChannelFillGraph 是折线
        // mLineStyle != lsLine 会导致闪烁，目前还不知道什么原因造成
        for (int i = otherSegmentData.size() - 1; i >= 0; --i)
            result.lineTo(otherSegmentData.at(i));
    }
    //! [1]

    // crop both vectors to ranges in which the keys overlap (which coord is key, depends on axisType):
    if (keyAxis->orientation() == Qt::Horizontal)
    {
        // x is key
        // crop lower bound:
        if (staticData->first().x() < croppedData->first().x()) // other one must be cropped
            qSwap(staticData, croppedData);
        const int lowBound = findIndexBelowX(croppedData, staticData->first().x());
        if (lowBound == -1) return result; // key ranges have no overlap
        //! [2] 以下为添加的内容
        QPointF firstPoint = QPointF(croppedData->at(0).x(), valueAxis->coordToPixel(valueAxis->range().upper));  // 注意这里只裁剪到了轴矩形的可见区域
        //! [2]
        croppedData->remove(0, lowBound);
        // set lowest point of cropped data to fit exactly key position of first static data point via linear interpolation:
        if (croppedData->size() < 2) return result; // need at least two points for interpolation
        double slope;
        if (!qFuzzyCompare(croppedData->at(1).x(), croppedData->at(0).x()))
            slope = (croppedData->at(1).y()-croppedData->at(0).y())/(croppedData->at(1).x()-croppedData->at(0).x());
        else
            slope = 0;
        (*croppedData)[0].setY(croppedData->at(0).y()+slope*(staticData->first().x()-croppedData->at(0).x()));
        (*croppedData)[0].setX(staticData->first().x());

        //! [3] 以下为添加的内容
        QPointF lastPoint = QPointF(staticData->first().x(), valueAxis->coordToPixel(valueAxis->range().lower));  // 注意这里只裁剪到了轴矩形的可见区域
        QPainterPath droppedPath;
        droppedPath.addRect(QRectF(firstPoint, lastPoint).normalized());
        result -= droppedPath;  // 裁掉多余区域
        //! [3]

        // crop upper bound:
        if (staticData->last().x() > croppedData->last().x()) // other one must be cropped
            qSwap(staticData, croppedData);
        int highBound = findIndexAboveX(croppedData, staticData->last().x());
        if (highBound == -1) return result; // key ranges have no overlap
        //! [4] 以下为添加的内容
        firstPoint = QPointF(croppedData->last().x(), valueAxis->coordToPixel(valueAxis->range().lower));  // 注意这里只裁剪到了轴矩形的可见区域
        //! [4]
        croppedData->remove(highBound+1, croppedData->size()-(highBound+1));
        // set highest point of cropped data to fit exactly key position of last static data point via linear interpolation:
        if (croppedData->size() < 2) return result; // need at least two points for interpolation
        const int li = croppedData->size()-1; // last index
        if (!qFuzzyCompare(croppedData->at(li).x(), croppedData->at(li-1).x()))
            slope = (croppedData->at(li).y()-croppedData->at(li-1).y())/(croppedData->at(li).x()-croppedData->at(li-1).x());
        else
            slope = 0;
        (*croppedData)[li].setY(croppedData->at(li-1).y()+slope*(staticData->last().x()-croppedData->at(li-1).x()));
        (*croppedData)[li].setX(staticData->last().x());

        //! [5] 以下为添加的内容
        lastPoint = QPointF(staticData->last().x(), valueAxis->coordToPixel(valueAxis->range().upper));
        droppedPath = QPainterPath();
        droppedPath.addRect(QRectF(firstPoint, lastPoint).normalized());
        result -= droppedPath;  // 裁掉多余区域
        //! [5]
    } else // mKeyAxis->orientation() == Qt::Vertical
    {
        // y is key
        // crop lower bound:
        if (staticData->first().y() < croppedData->first().y()) // other one must be cropped
            qSwap(staticData, croppedData);
        int lowBound = findIndexBelowY(croppedData, staticData->first().y());
        if (lowBound == -1) return result; // key ranges have no overlap
        //! [6] 以下为添加的内容
        QPointF firstPoint = QPointF(valueAxis->coordToPixel(valueAxis->range().upper), croppedData->first().y());
        //! [6]
        croppedData->remove(0, lowBound);
        // set lowest point of cropped data to fit exactly key position of first static data point via linear interpolation:
        if (croppedData->size() < 2) return result; // need at least two points for interpolation
        double slope;
        if (!qFuzzyCompare(croppedData->at(1).y(), croppedData->at(0).y())) // avoid division by zero in step plots
            slope = (croppedData->at(1).x()-croppedData->at(0).x())/(croppedData->at(1).y()-croppedData->at(0).y());
        else
            slope = 0;
        (*croppedData)[0].setX(croppedData->at(0).x()+slope*(staticData->first().y()-croppedData->at(0).y()));
        (*croppedData)[0].setY(staticData->first().y());

        //! [7] 以下为添加的内容
        QPointF lastPoint = QPointF(valueAxis->coordToPixel(valueAxis->range().lower), staticData->first().y());
        QPainterPath droppedPath;
        droppedPath.addRect(QRectF(firstPoint, lastPoint).normalized());
        result -= droppedPath;
        //! [7]

        // crop upper bound:
        if (staticData->last().y() > croppedData->last().y()) // other one must be cropped
            qSwap(staticData, croppedData);
        int highBound = findIndexAboveY(croppedData, staticData->last().y());
        if (highBound == -1) return result; // key ranges have no overlap
        //! [8] 以下为添加的内容
        firstPoint = QPointF(valueAxis->coordToPixel(valueAxis->range().lower), croppedData->last().y());
        //! [8]
        croppedData->remove(highBound+1, croppedData->size()-(highBound+1));
        // set highest point of cropped data to fit exactly key position of last static data point via linear interpolation:
        if (croppedData->size() < 2) return result; // need at least two points for interpolation
        int li = croppedData->size()-1; // last index
        if (!qFuzzyCompare(croppedData->at(li).y(), croppedData->at(li-1).y())) // avoid division by zero in step plots
            slope = (croppedData->at(li).x()-croppedData->at(li-1).x())/(croppedData->at(li).y()-croppedData->at(li-1).y());
        else
            slope = 0;
        (*croppedData)[li].setX(croppedData->at(li-1).x()+slope*(staticData->last().y()-croppedData->at(li-1).y()));
        (*croppedData)[li].setY(staticData->last().y());

        //! [9] 以下为添加的内容
        lastPoint = QPointF(valueAxis->coordToPixel(valueAxis->range().upper), staticData->last().y());
        droppedPath = QPainterPath();
        droppedPath.addRect(QRectF(firstPoint, lastPoint).normalized());
        result -= droppedPath;
        //! [9]
    }
    return result;
}
```

![与其它QCPGraph围成的区域](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC1kOTFjMjE3NTNkMTIwZjAzLnBuZw?x-oss-process=image/format,png)
