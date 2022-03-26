#include "mainwindow.h"
#include "qcustomplot.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QCustomPlot* customPlot = new QCustomPlot;
    setCentralWidget(customPlot);

    setupAdvancedAxesDemo(customPlot);
}

void MainWindow::setupAdvancedAxesDemo(QCustomPlot *customPlot)
{
    customPlot->plotLayout()->clear();   // 首先清空默认的轴矩形，让我们从头开始
    QCPAxisRect *wideAxisRect = new QCPAxisRect(customPlot, true);   // 还记得setupDefaultAxes为true时的作用吗，忘了的话翻上去看吧
    wideAxisRect->setupFullAxesBox(true);    // 让四个轴关联，并全部显示出来
    wideAxisRect->addAxis(QCPAxis::atLeft)->setTickLabelColor(QColor("#6050F8"));  // 在左边多添加一个轴并且设置它的颜色

    QCPLayoutGrid *subLayout = new QCPLayoutGrid;
    customPlot->plotLayout()->addElement(0, 0, wideAxisRect);     // 在第一行添加轴矩形
    customPlot->plotLayout()->addElement(1, 0, subLayout);        // 在第二行添加一个子布局，后面会添加两个轴矩形在里面

    QCPAxisRect *subRectLeft = new QCPAxisRect(customPlot, false);   // 不配置轴
    QCPAxisRect *subRectRight = new QCPAxisRect(customPlot, false);

    // 让右边的轴矩形固定大小
    subRectRight->setMaximumSize(100, 100);
    subRectRight->setMinimumSize(100, 100);

    subLayout->addElement(0, 0, subRectLeft);                      // 在第一列添加轴矩形
    subLayout->addElement(0, 1, subRectRight);                     // 在第二列添加轴矩形

    subRectLeft->addAxes(QCPAxis::atBottom | QCPAxis::atLeft);     // 添加下轴和左轴
    subRectRight->addAxes(QCPAxis::atBottom | QCPAxis::atRight);   // 添加下轴和右轴
    subRectLeft->axis(QCPAxis::atLeft)->ticker()->setTickCount(2); // 设置轴的刻度为一个固定的步进值
    subRectRight->axis(QCPAxis::atRight)->ticker()->setTickCount(2);
    subRectLeft->axis(QCPAxis::atBottom)->grid()->setVisible(true);

    // 保持一个好的习惯，将它们放置在相应的层
    foreach (auto *rect, customPlot->axisRects()) {
        foreach (auto *axis, rect->axes()) {
            axis->setLayer("axes");
            axis->grid()->setLayer("grid");
        }
    }

    // 准备数据
    QVector<QCPGraphData> dataCos(21), dataGauss(50), dataRandom(100);
    QVector<double> x3, y3;
    qsrand(3);
    for (int i=0; i<dataCos.size(); ++i) {
        dataCos[i].key = i/(double)(dataCos.size()-1)*10-5.0;
        dataCos[i].value = qCos(dataCos[i].key);
    }
    for (int i=0; i<dataGauss.size(); ++i) {
        dataGauss[i].key = i/(double)dataGauss.size()*10-5.0;
        dataGauss[i].value = qExp(-dataGauss[i].key*dataGauss[i].key*0.2)*1000;
    }
    for (int i=0; i<dataRandom.size(); ++i) {
        dataRandom[i].key = i/(double)dataRandom.size()*10;
        dataRandom[i].value = qrand()/(double)RAND_MAX-0.5+dataRandom[qMax(0, i-1)].value;
    }
    x3 << 1 << 2 << 3 << 4;
    y3 << 2 << 2.5 << 4 << 1.5;

    // mainGraphCos和 mainGraphGauss 共享下轴，但是它们的左轴不同
    QCPGraph *mainGraphCos = customPlot->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));
    mainGraphCos->data()->set(dataCos);
    mainGraphCos->valueAxis()->setRange(-1, 1);
    mainGraphCos->rescaleKeyAxis();
    mainGraphCos->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
    mainGraphCos->setPen(QPen(QColor(120, 120, 120), 2));

    QCPGraph *mainGraphGauss = customPlot->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft, 1));
    mainGraphGauss->data()->set(dataGauss);
    mainGraphGauss->setPen(QPen(QColor("#8070B8"), 2));
    mainGraphGauss->setBrush(QColor(110, 170, 110, 30));
    mainGraphCos->setChannelFillGraph(mainGraphGauss);
    mainGraphCos->setBrush(QColor(255, 161, 0, 50));
    mainGraphGauss->valueAxis()->setRange(0, 1000);
    mainGraphGauss->rescaleKeyAxis();

    QCPGraph *subGraphRandom = customPlot->addGraph(subRectLeft->axis(QCPAxis::atBottom), subRectLeft->axis(QCPAxis::atLeft));
    subGraphRandom->data()->set(dataRandom);
    subGraphRandom->setLineStyle(QCPGraph::lsImpulse);
    subGraphRandom->setPen(QPen(QColor("#FFA100"), 1.5));
    subGraphRandom->rescaleAxes();

    QCPBars *subBars = new QCPBars(subRectRight->axis(QCPAxis::atBottom), subRectRight->axis(QCPAxis::atRight));
    subBars->setWidth(3/(double)x3.size());
    subBars->setData(x3, y3);
    subBars->setPen(QPen(Qt::black));
    subBars->setAntialiased(false);
    subBars->setAntialiasedFill(false);
    subBars->setBrush(QColor("#705BE8"));
    subBars->keyAxis()->setSubTicks(false);
    subBars->rescaleAxes();

    // 给柱状图一个固定步进轴
    QSharedPointer<QCPAxisTickerFixed> intTicker(new QCPAxisTickerFixed);
    intTicker->setTickStep(1.0);
    intTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    subBars->keyAxis()->setTicker(intTicker);
}

