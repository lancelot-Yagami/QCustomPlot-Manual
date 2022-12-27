#ifndef QCPPLOT_H
#define QCPPLOT_H

#include "qcpsizehandlemanager.h"

class QCP_LIB_DECL QCPPlot : public QCustomPlot
{
    Q_OBJECT
public:
    explicit QCPPlot(QWidget* parent = nullptr);

    QCPSizeHandleManager* sizeHandleManager() const { return mSizeHandleManager; }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

protected:
    QCPSizeHandleManager* mSizeHandleManager;
    QPointF mLastMousePos;
};

#endif // QCPPLOT_H
