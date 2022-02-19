#include "mainwindow.h"
#include "qcustomplot.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QCustomPlot* customPlot = new QCustomPlot;
    setCentralWidget(customPlot);

    setupQuadraticDemo(customPlot);
}

void MainWindow::setupQuadraticDemo(QCustomPlot *customPlot)
{
    QVector<double> x(101), y(101);
    for (int i = 0; i < 101; ++i) {
        x[i] = i / 50.0 - 1;                                                               // -1 到 1
        y[i] = x[i] * x[i];
    }

    customPlot->addGraph();                                                  // 添加一个曲线图QGraph
    customPlot->graph(0)->setData(x, y);                             // 为曲线图添加数据
    customPlot->graph(0)->setName("第一个示例");          // 设置曲线图的名字
    customPlot->xAxis->setLabel("x");                                 // 设置x轴的标签
    customPlot->yAxis->setLabel("y");
    customPlot->xAxis->setRange(-1, 1);                             // 设置x轴的范围为(-1,1)
    customPlot->yAxis->setRange(0, 1);
    customPlot->legend->setVisible(true);                        // 显示图例
}
