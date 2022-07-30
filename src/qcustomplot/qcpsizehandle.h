#ifndef QCPSIZEHANDLE_H
#define QCPSIZEHANDLE_H

#include "qcustomplot.h"

class QCP_LIB_DECL QCPSizeHandle : public QCPAbstractItem
{
    Q_OBJECT
public:
    explicit QCPSizeHandle(QCustomPlot *parentPlot, QCPAbstractItem* attachItem);
    ~QCPSizeHandle();

    void setBrush(const QBrush &brush);
    void setSelectedBrush(const QBrush &brush);
    void setHoveredBrush(const QBrush &brush);
    void setSize(double size);

    QCPAbstractItem* attachItem() const { return mAttachItem; }

    QCPItemPosition * const position;

public:
    virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details = nullptr) const Q_DECL_OVERRIDE;

protected:
    bool mHovered;
    double mSize;
    QBrush mBrush;
    QBrush mSelectedBrush;
    QBrush mHoveredBrush;
    QCPAbstractItem* mAttachItem;

    virtual void draw(QCPPainter *painter) Q_DECL_OVERRIDE;
//    virtual void hoverEnterEvent(QHoverEvent *event) Q_DECL_OVERRIDE;
//    virtual void hoverLeaveEvent(QHoverEvent *event) Q_DECL_OVERRIDE;

    QBrush mainBrush() const;
};

#endif // QCPSIZEHANDLE_H
