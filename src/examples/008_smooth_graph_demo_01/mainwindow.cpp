#include "mainwindow.h"
#include "qcpsmoothcurve.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mCustomPlot = new QCustomPlot;
    mCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    setCentralWidget(mCustomPlot);
    setupSmoothCurveDemo(mCustomPlot);
}

void MainWindow::setupSmoothCurveDemo(QCustomPlot *customPlot)
{
    QVector<double> xdata = { 1, 2, 3, 4, 5, 6, 7 };
    QVector<double> ydata = { 820, 932, 901, 934, 1290, 1330, 1320 };

    QCPSmoothCurve *graph = new QCPSmoothCurve(customPlot->xAxis, customPlot->yAxis);
    graph->setPen(QPen(Qt::red, 2));
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::red), QColor(Qt::white), 6));
    graph->setData(xdata, ydata);
    graph->setSmooth(true);

    customPlot->xAxis->setRange(0, 8);
    customPlot->yAxis->setRange(0, 1500);
}

