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
    graph->setBrush(QColor(208,110,107));                  // 设置了填充色即可填充曲线与0点线之间的区域
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::red), QColor(Qt::white), 6));
    graph->setData(xdata, ydata);
    graph->setSmooth(true);

    double offset = 500;
    QVector<double> ydata1 = { 820 - offset, 880 - offset, 901 - offset, 934 - offset, 1290 - offset, 1330 - offset, 1320  - offset };
    QCPSmoothCurve *graph1 = new QCPSmoothCurve(customPlot->xAxis, customPlot->yAxis);
    graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::blue), QColor(Qt::white), 6));
    graph1->setData(xdata, ydata1);
    graph1->setSmooth(true);

    graph->setChannelFillGraph(graph1);   // 设置与其它曲线填充

    customPlot->xAxis->setRange(0, 8);
    customPlot->yAxis->setRange(0, 1500);
}

