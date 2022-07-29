#include "qcpsmoothcurve.h"

class SmoothCurveGenerator
{
protected:
    static QPainterPath generateSmoothCurveImp(const QVector<QPointF> &points) {
        QPainterPath path;
        int len = points.size();

        if (len < 2) {
            return path;
        }

        QVector<QPointF> firstControlPoints;
        QVector<QPointF> secondControlPoints;
        calculateControlPoints(points, &firstControlPoints, &secondControlPoints);

        path.moveTo(points[0].x(), points[0].y());

        // Using bezier curve to generate a smooth curve.
        for (int i = 0; i < len - 1; ++i) {
            path.cubicTo(firstControlPoints[i], secondControlPoints[i], points[i+1]);
        }

        return path;
    }
public:
    static QPainterPath generateSmoothCurve(const QVector<QPointF> &points) {
        QPainterPath result;
        QVector<QPointF> lineData(QVector<QPointF>(points.constBegin(), points.constEnd()));
        result.addPath(generateSmoothCurveImp(lineData));
        return result;
    }

    static QPainterPath generateSmoothCurve(const QPainterPath &basePath, const QVector<QPointF> &points) {
        if (points.isEmpty()) return basePath;

        QPainterPath path = basePath;
        int len = points.size();
        if (len == 1) {
            path.lineTo(points.at(0));
            return path;
        }

        QVector<QPointF> firstControlPoints;
        QVector<QPointF> secondControlPoints;
        calculateControlPoints(points, &firstControlPoints, &secondControlPoints);

        path.lineTo(points.at(0));
        for (int i = 0; i < len - 1; ++i)
            path.cubicTo(firstControlPoints[i], secondControlPoints[i], points[i+1]);

        return path;
    }

    static void calculateFirstControlPoints(double *&result, const double *rhs, int n) {
        result = new double[n];
        double *tmp = new double[n];
        double b = 2.0;
        result[0] = rhs[0] / b;

        // Decomposition and forward substitution.
        for (int i = 1; i < n; i++) {
            tmp[i] = 1 / b;
            b = (i < n - 1 ? 4.0 : 3.5) - tmp[i];
            result[i] = (rhs[i] - result[i - 1]) / b;
        }

        for (int i = 1; i < n; i++) {
            result[n - i - 1] -= tmp[n - i] * result[n - i]; // Backsubstitution.
        }

        delete[] tmp;
    }

    static void calculateControlPoints(const QVector<QPointF> &knots,
                                       QVector<QPointF> *firstControlPoints,
                                       QVector<QPointF> *secondControlPoints) {
        int n = knots.size() - 1;

        firstControlPoints->reserve(n);
        secondControlPoints->reserve(n);

        for (int i = 0; i < n; ++i) {
            firstControlPoints->append(QPointF());
            secondControlPoints->append(QPointF());
        }

        if (n == 1) {
            // Special case: Bezier curve should be a straight line.
            // P1 = (2P0 + P3) / 3
            (*firstControlPoints)[0].rx() = (2 * knots[0].x() + knots[1].x()) / 3;
            (*firstControlPoints)[0].ry() = (2 * knots[0].y() + knots[1].y()) / 3;

            // P2 = 2P1 – P0
            (*secondControlPoints)[0].rx() = 2 * (*firstControlPoints)[0].x() - knots[0].x();
            (*secondControlPoints)[0].ry() = 2 * (*firstControlPoints)[0].y() - knots[0].y();

            return;
        }

        // Calculate first Bezier control points
        double *xs = nullptr;
        double *ys = nullptr;
        double *rhsx = new double[n]; // Right hand side vector
        double *rhsy = new double[n]; // Right hand side vector

        // Set right hand side values
        for (int i = 1; i < n - 1; ++i) {
            rhsx[i] = 4 * knots[i].x() + 2 * knots[i + 1].x();
            rhsy[i] = 4 * knots[i].y() + 2 * knots[i + 1].y();
        }
        rhsx[0] = knots[0].x() + 2 * knots[1].x();
        rhsx[n - 1] = (8 * knots[n - 1].x() + knots[n].x()) / 2.0;
        rhsy[0] = knots[0].y() + 2 * knots[1].y();
        rhsy[n - 1] = (8 * knots[n - 1].y() + knots[n].y()) / 2.0;

        // Calculate first control points coordinates
        calculateFirstControlPoints(xs, rhsx, n);
        calculateFirstControlPoints(ys, rhsy, n);

        // Fill output control points.
        for (int i = 0; i < n; ++i) {
            (*firstControlPoints)[i].rx() = xs[i];
            (*firstControlPoints)[i].ry() = ys[i];

            if (i < n - 1) {
                (*secondControlPoints)[i].rx() = 2 * knots[i + 1].x() - xs[i + 1];
                (*secondControlPoints)[i].ry() = 2 * knots[i + 1].y() - ys[i + 1];
            } else {
                (*secondControlPoints)[i].rx() = (knots[n].x() + xs[n - 1]) / 2;
                (*secondControlPoints)[i].ry() = (knots[n].y() + ys[n - 1]) / 2;
            }
        }

        delete xs;
        delete ys;
        delete[] rhsx;
        delete[] rhsy;
    }
};


QCPSmoothCurve::QCPSmoothCurve(QCPAxis *keyAxis, QCPAxis *valueAxis)
    : QCPGraph(keyAxis, valueAxis)
    , mSmooth(true)
{

}

void QCPSmoothCurve::draw(QCPPainter *painter)
{
    if (!mKeyAxis || !mValueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
    if (mKeyAxis.data()->range().size() <= 0 || mDataContainer->isEmpty()) return;
    if (mLineStyle == lsNone && mScatterStyle.isNone()) return;

    if (mSmooth && mLineStyle == lsLine) {
        QVector<QCPGraphData> data(mDataContainer->begin(), mDataContainer->end());
        mLines = dataToLines(data);
        mSegments = getNonNanSegments(&mLines, mKeyAxis->orientation());
        mSmoothPaths = QVector<QPainterPath>();
        mSmoothPaths.reserve(mSegments.count());
        for (int i = 0; i < mSegments.count(); ++i) {
            const auto& segment = mSegments.at(i);
            QVector<QPointF> points(mLines.constBegin() + segment.begin(), mLines.constBegin() + segment.end());
            mSmoothPaths.append(SmoothCurveGenerator::generateSmoothCurve(points));
        }
        qDebug() << mSegments.count() << mSmoothPaths.count() << mLines.count();
    }

    QCPGraph::draw(painter);

    mSegments = QVector<QCPDataRange>();
    mSmoothPaths = QVector<QPainterPath>();
    mLines = QVector<QPointF>();
}

void QCPSmoothCurve::drawLinePlot(QCPPainter *painter, const QVector<QPointF> &lines) const
{
    if (painter->pen().style() != Qt::NoPen && painter->pen().color().alpha() != 0) {
        applyDefaultAntialiasingHint(painter);
        if (mSmooth && mLineStyle == lsLine) {
            QPainterPath clipPath = painter->clipPath();
            QPainterPath clipRect = getLinesClipPath(lines);

            painter->setClipPath(clipPath.intersected(clipRect));
            for (int i = 0; i < mSmoothPaths.count(); ++i) {
                if (mSmoothPaths.at(i).intersects(clipRect))
                    painter->drawPath(mSmoothPaths.at(i));
            }

            painter->setClipPath(clipPath);
        }
        else
            drawPolyline(painter, lines);
    }
}

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
            if (mSmooth && mLineStyle == lsLine) {
                QPainterPath clipPath = painter->clipPath();
                QPainterPath clipRect;
                auto path = getSmoothFillPath(lines, segments.at(i), &clipRect);
                painter->setClipPath(clipPath.intersected(clipRect));
                painter->drawPath(path);   // 平滑曲线
                painter->setClipPath(clipPath);
            }
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
                if ((mLineStyle == lsLine && channelFillGraph->mLineStyle == lsLine) && (mSmooth || channelFillGraph->mSmooth))
                    painter->drawPath(getSmoothChannelFillPath(lines, segmentPairs.at(i).first, &otherLines, segmentPairs.at(i).second));  // 平滑曲线
                else
                    painter->drawPolygon(getChannelFillPolygon(lines, segmentPairs.at(i).first, &otherLines, segmentPairs.at(i).second));   // 折线
            }
        }
    }
}

QPainterPath QCPSmoothCurve::getSmoothFillPath(const QVector<QPointF> *lineData, QCPDataRange segment, QPainterPath *clipPath) const
{
    // 只有一个点构不成填充区域
    if (segment.size() < 2)
        return QPainterPath();

    *clipPath = getLinesClipPath(getFillPolygon(lineData, segment));

    QPainterPath result;
    for (int i = 0; i < mSmoothPaths.count(); ++i) {
        QCPDataRange subSegment = mSegments.at(i);
        if (subSegment.size() < 2)
            continue;

        QPointF start = getFillBasePoint(mLines.at(subSegment.begin()));
        QPointF end = getFillBasePoint(mLines.at(subSegment.end() - 1));
        QPainterPath path = mSmoothPaths.at(i);
        path.lineTo(end);
        path.lineTo(start);
        path.lineTo(mLines.at(subSegment.begin()));
        result.addPath(path);

    }
    return result;
}

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
    if (mSmooth)
        result = SmoothCurveGenerator::generateSmoothCurve(thisSegmentData);
    else
        result.addPolygon(thisSegmentData);
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

QPainterPath QCPSmoothCurve::getLinesClipPath(const QPolygonF &polygon) const
{
    QPainterPath result;
    QRectF br = polygon.boundingRect();
    if (br.isNull()) return result;

    if (mKeyAxis->orientation() == Qt::Horizontal) {
        QPointF firstPoint = QPointF(br.x(), mValueAxis->coordToPixel(mValueAxis->range().upper));
        QPointF lastPoint = QPointF(br.x() + br.width(), mValueAxis->coordToPixel(mValueAxis->range().lower));
        result.addRect(QRectF(firstPoint, lastPoint).normalized());
    } else {
        QPointF firstPoint = QPointF(mValueAxis->coordToPixel(mValueAxis->range().lower), br.y());
        QPointF lastPoint = QPointF(mValueAxis->coordToPixel(mValueAxis->range().upper), br.y() + br.height());
        result.addRect(QRectF(firstPoint, lastPoint).normalized());
    }
    return result;
}

