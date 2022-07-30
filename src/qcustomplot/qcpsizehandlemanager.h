#ifndef QCPSIZEHANDLEMANAGER_H
#define QCPSIZEHANDLEMANAGER_H

#include "qcpsizehandle.h"

class QCPSizeHandle;
class QCPSizeHandleManager : public QCPAbstractItem
{
    Q_OBJECT
public:
    explicit QCPSizeHandleManager(QCustomPlot *parent);

    bool addItem(QCPAbstractItem* item);
    void removeItem(QCPAbstractItem* item);

    void setLineVisible(QCPAbstractItem* item, bool visible);
    void setLinePen(QCPAbstractItem* item, const QPen& pen);
    void setItemMovable(QCPAbstractItem* item, bool movable);
    void setItemResizable(QCPAbstractItem* item, bool resizable);

    double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details = nullptr) const override;

public slots:
    void onItemMove(QCPAbstractItem* item, const QPointF &delta);
    void onItemResize(QCPSizeHandle* sizeHandle, const QPointF &delta);

protected:
    void draw(QCPPainter *painter) override;

protected:
    struct ItemData {
        bool lineVisible = false;
        bool movable    = true;
        bool resizable    = true;
        QPen linePen      = QPen(Qt::blue, 1, Qt::DashLine);
        QList<QCPSizeHandle*> handles;
    };

    QMap<QPointer<QCPAbstractItem>, QSharedPointer<ItemData>> mItems;
};

#endif // QCPSIZEHANDLEMANAGER_H
