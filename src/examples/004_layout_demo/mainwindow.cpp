#include "mainwindow.h"

#include <QVBoxLayout>
#include <QComboBox>

#include "qcustomplot.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mCustomPlot = new QCustomPlot;

    setWindowTitle("QCustomPlot: Layout Demo");

    QWidget* w = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    w->setLayout(layout);
    setCentralWidget(w);

    QStringList items;
    items << "对齐位置图例" << "自由位置图例" << "图例在QCustomPlot中";
    QComboBox* comboBox = new QComboBox;
    comboBox->addItems(items);
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::setupDemo);

    layout->addWidget(comboBox);
    layout->addWidget(mCustomPlot);
    setupDemo(0);
}

void MainWindow::reset()
{
    delete mCustomPlot;
    mCustomPlot = new QCustomPlot;
    centralWidget()->layout()->addWidget(mCustomPlot);

    mOrientation = Qt::Vertical;
    mIsFreeLegend = false;
    mIsLegendInCustomPlot = false;
}

void MainWindow::setupDemo(int index)
{
    reset();

    switch (index) {
    case 0: setupLayoutDemo(mCustomPlot); break;
    case 1: mIsFreeLegend = true; setupLayoutDemo(mCustomPlot); break;
    case 2: mIsLegendInCustomPlot = true; setupLayoutDemo(mCustomPlot); break;
    }
}

void MainWindow::setupQuadraticDemo(QCustomPlot *customPlot)
{
    QVector<double> x(101), y(101);
    for (int i = 0; i < 101; ++i) {
        x[i] = i / 50.0 - 1;                                                               // -1 到 1
        y[i] = x[i] * x[i];
    }

    auto keyAxis = customPlot->xAxis;
    auto valueAxis = customPlot->yAxis;

    if (mOrientation == Qt::Horizontal) {
        keyAxis = customPlot->yAxis;
        valueAxis = customPlot->xAxis;
    }

    customPlot->addGraph(keyAxis, valueAxis);                                                  // 添加一个曲线图QGraph
    customPlot->graph(0)->setName("第一个示例");                                        // 设置曲线图的名字，用于QCPLegend显示

    customPlot->xAxis->setLabel("x");                                                                // 设置x轴的标签
    customPlot->yAxis->setLabel("y");
    customPlot->graph(0)->setData(x, y);
    customPlot->rescaleAxes();
}

void MainWindow::setupLayoutDemo(QCustomPlot* customPlot)
{
    setupQuadraticDemo(customPlot);
    customPlot->legend->setVisible(true);                                                                                                 // 因为图例默认为隐藏的，所以让其显示
    // 设置图例标题
    QCPTextElement *title = new QCPTextElement(customPlot, "这是图例标题", QFont("sans", 10, QFont::Bold));
    title->setMargins(QMargins(0, 6, 0, 10));      // 为了效果更好，添加一些边距
    title->setLayer("legend");                     // 一定要把标题的层设置为legend层
    customPlot->legend->insertRow(0);              // 插入一行
    customPlot->legend->addElement(0, 0, title);   // 在第一行第一列添加标题

    if (mIsFreeLegend) {
        customPlot->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
        customPlot->axisRect()->insetLayout()->setInsetRect(0, QRectF(0.6, 0.0, 0.2, 0.2));         // 这里按比例给出图例的位置，在QCPAxisRect的(60%,40%)的位置，宽高分别为QCPAxisRect的(20%,20%)宽高的大小
    } else {
        customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignHCenter);   // 对齐于QCPAxisRect中间的顶部
    }

    if (mIsLegendInCustomPlot) {
        QCPAxisRect* legendAxisRect = new QCPAxisRect(customPlot, false);                              // 不需要配置轴，因为我们只是把图例放在这里
        legendAxisRect->insetLayout()->addElement(customPlot->legend, QRectF(0, 0, 1, 1));  // 添加图例，充满整个legendAxisRect

        // 添加到QCustomPlot中
        customPlot->plotLayout()->insertRow(0);
        customPlot->plotLayout()->addElement(0, 0, legendAxisRect);
        customPlot->plotLayout()->setRowStretchFactor(0, 0.1);

        // 设置边距对齐
        QCPMarginGroup* marginGroup = new QCPMarginGroup(customPlot);
        customPlot->axisRect(0)->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
        customPlot->axisRect(1)->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
    }

    // 设置图表标题
    customPlot->plotLayout()->insertRow(0);
    customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(customPlot, "这里是QCustomPlot的标题"));
    customPlot->plotLayout()->setMargins(QMargins(0, 10, 0, 10));
}

