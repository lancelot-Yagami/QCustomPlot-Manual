#ifndef QCPSMOOTHCURVE_H
#define QCPSMOOTHCURVE_H

#include "qcustomplot.h"

class QCP_LIB_DECL QCPSmoothCurve : public QCPGraph
{
    Q_OBJECT
public:
    explicit QCPSmoothCurve(QCPAxis *keyAxis, QCPAxis *valueAxis);

    void setSmooth(bool smooth) { mSmooth = smooth; }

protected:
    void draw(QCPPainter *painter) override;
    void drawLinePlot(QCPPainter *painter, const QVector<QPointF> &lines) const override;
    void drawFill(QCPPainter *painter, QVector<QPointF> *lines) const override;

protected:
    QPainterPath getSmoothFillPath(const QVector<QPointF> *lineData, QCPDataRange segment, QPainterPath *clipPath) const;
    QPainterPath getSmoothChannelFillPath(const QVector<QPointF> *thisData, QCPDataRange thisSegment,
                                                          const QVector<QPointF> *otherData, QCPDataRange otherSegment) const;
    QRectF getLinesRect(const QVector<QPointF>& lineData);
    QPainterPath getLinesClipPath(const QPolygonF& polygon) const;

protected:
    bool mSmooth;
    QVector<QPainterPath> mSmoothPaths;
    QVector<QCPDataRange> mSegments;
    QVector<QPointF> mLines;
};

#endif // QCPSMOOTHCURVE_H
