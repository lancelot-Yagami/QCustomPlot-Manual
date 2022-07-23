#include "mainwindow.h"
#include "qcustomplot.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mCustomPlot = new QCustomPlot;
    setCentralWidget(mCustomPlot);

    setupLemniscateDemo(mCustomPlot);
}

void MainWindow::setupLemniscateDemo(QCustomPlot *customPlot)
{
    QCPCurve *curve = new QCPCurve(customPlot->xAxis, customPlot->yAxis);
    curve->setPen(QPen(Qt::red));

    // 由于我们是动态图，并且其范围不变化，所以预先设置好了范围
    customPlot->xAxis->setRange(-2.5, 12.5);
    customPlot->yAxis->setRange(-2.8, 2.8);

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(dynamicLemniscateSlot()));
    dataTimer.start(10);      // 10ms刷新一次，实际上QTimer可能达不到这个精度
}

void MainWindow::dynamicLemniscateSlot()
{
    static double t = -M_PI;           // 定义起始t
    static bool reversed = false;   // 为了重复绘制

    QCPCurve *curve = static_cast<QCPCurve *>(mCustomPlot->plottable(0));

    //  伯努利双纽线(无穷大的符号)
    double x = -686*qCos(t) + 735*qCos(2*t) + 490*qSqrt(qPow(qSin(t), 4)) - 265;
    x /= (280*qCos(t) - 298);
    double y = 5*qSqrt(qPow(qSin(t), 4))*(10/qSin(t) - 7/qTan(t)) + 3*(35*qCos(t) - 33)*qSin(t);
    y = 7*y/(140*qCos(t) - 149);

    if (!reversed) {        // 还未绘制完成
        t += 0.01;
        curve->data()->add(QCPCurveData(t, x, y));
    } else {               // 绘制已经完成，倒退回去
        t -= 0.01;
        curve->data()->removeAfter(t);        // 这里不能用remove，因为精度原因，remove并不能精确匹配，导致无法删除数据点
    }

    mCustomPlot->replot();

    // 重复绘制
    if (t >= M_PI) {
        t = M_PI;
        reversed = true;
    } else if (reversed && t <= -M_PI) {
        t = -M_PI;
        reversed = false;
    }
}
