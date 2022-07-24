﻿#ifndef QCPSMOOTHCURVE_H
#define QCPSMOOTHCURVE_H

#include "qcustomplot.h"

class QCP_LIB_DECL QCPSmoothCurve : public QCPGraph
{
    Q_OBJECT
public:
    explicit QCPSmoothCurve(QCPAxis *keyAxis, QCPAxis *valueAxis);

    void setSmooth(bool smooth) { mSmooth = smooth; }

protected:
    void drawLinePlot(QCPPainter *painter, const QVector<QPointF> &lines) const override;
    void drawFill(QCPPainter *painter, QVector<QPointF> *lines) const override;
    void getOptimizedLineData(QVector<QCPGraphData> *lineData, const QCPGraphDataContainer::const_iterator &begin,
                              const QCPGraphDataContainer::const_iterator &end) const override;

protected:
    QPainterPath getSmoothFillPath(const QVector<QPointF> *lineData, QCPDataRange segment) const;
    QPainterPath getSmoothChannelFillPath(const QVector<QPointF> *thisData, QCPDataRange thisSegment,
                                                          const QVector<QPointF> *otherData, QCPDataRange otherSegment) const;

protected:
    bool mSmooth;
};

#endif // QCPSMOOTHCURVE_H
