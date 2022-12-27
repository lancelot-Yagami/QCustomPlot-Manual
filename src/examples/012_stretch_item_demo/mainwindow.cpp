#include "mainwindow.h"
#include "qcpplot.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mCustomPlot = new QCPPlot;
    setCentralWidget(mCustomPlot);
    setupStretchItemDemo(mCustomPlot);
}

void MainWindow::setupStretchItemDemo(QCustomPlot *plot)
{
    auto customPlot = qobject_cast<QCPPlot*>(plot);
    customPlot->setInteractions(QCP::iSelectItems | QCP::iRangeZoom | QCP::iRangeDrag);
    QCPItemRect *rect = new QCPItemRect(customPlot);
    customPlot->sizeHandleManager()->addItem(rect);
    rect->setBrush(Qt::red);
    rect->setSelectedBrush(Qt::red);

    QCPItemLine *line = new QCPItemLine(customPlot);
    customPlot->sizeHandleManager()->addItem(line);

    QCPItemCurve *curve = new QCPItemCurve(customPlot);
    customPlot->sizeHandleManager()->addItem(curve);
    customPlot->sizeHandleManager()->setLineVisible(curve, true);

    QCPItemText *text = new QCPItemText(customPlot);
    text->setText(QString::fromLocal8Bit("试着拖动我！"));
    customPlot->sizeHandleManager()->addItem(text);

    QCPItemBracket *bracket = new QCPItemBracket(customPlot);
    customPlot->sizeHandleManager()->addItem(bracket);

    QCPItemEllipse *ellipse = new QCPItemEllipse(customPlot);
    customPlot->sizeHandleManager()->addItem(ellipse);
    customPlot->sizeHandleManager()->setLineVisible(ellipse, true);

//    QCPItemPixmap *pixmap = new QCPItemPixmap(customPlot);
//    pixmap->setPixmap(QPixmap(":/sun.png"));
//    customPlot->sizeHandleManager()->addItem(pixmap);

    QCPItemStraightLine *straightLine = new QCPItemStraightLine(customPlot);
    customPlot->sizeHandleManager()->addItem(straightLine);

    QCPItemTracer *tracer = new QCPItemTracer(customPlot);
    customPlot->sizeHandleManager()->addItem(tracer);
}
