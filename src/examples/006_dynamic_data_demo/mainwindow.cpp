#include "mainwindow.h"
#include "qcustomplot.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mCustomPlot = new QCustomPlot;
    setCentralWidget(mCustomPlot);

    setupDynamicData(mCustomPlot);
}

void MainWindow::setupDynamicData(QCustomPlot *customPlot)
{
    customPlot->plotLayout()->insertRow(0);
    customPlot->plotLayout()->insertColumn(0);

    QCPAxisRect *legendRect = new QCPAxisRect(customPlot, false);  // 不需要配置轴，因为我们只是把图例放在这里
    legendRect->setAutoMargins(QCP::msNone); // 不计算边距
    legendRect->insetLayout()->addElement(customPlot->legend, Qt::AlignCenter);  // 放置图例
    customPlot->legend->setVisible(true);
    customPlot->legend->setFillOrder(QCPLayoutGrid::foColumnsFirst); // 横向图例

    customPlot->axisRect()->setAutoMargins(QCP::msBottom | QCP::msRight); // 只计算下边距和右边距
    customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(customPlot, "动态数据", QFont("sans", 12, QFont::Bold)));
    customPlot->plotLayout()->addElement(0, 1, legendRect);
    customPlot->plotLayout()->setRowStretchFactor(0, 0.1);    // 让其尽可能小一点
    customPlot->plotLayout()->setColumnStretchFactor(0, 0.1);

    // 设置柱状图
    QCPBars *bar = new QCPBars(customPlot->xAxis, customPlot->yAxis2);
    bar->setName("预购队列");
    bar->setPen(Qt::NoPen);
    bar->setBrush(QColor(194, 53, 49));

    // 设置折线图
    QCPGraph *graph = customPlot->addGraph(customPlot->xAxis, customPlot->yAxis);
    graph->setName("最新成交价");
    graph->setPen(QPen(QColor(47, 69, 84), 2));
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(47, 69, 84), QColor(Qt::white), 4));

    customPlot->yAxis->setLabel("价格");
    customPlot->yAxis->setRange(0, 30);
    customPlot->yAxis->setSubTicks(false);   // 设置子刻度线不显示
    customPlot->yAxis->setTickLength(0, 6);  // 设置刻度线内外的长度
    customPlot->yAxis2->setLabel("预购量");
    customPlot->yAxis2->setVisible(true);
    customPlot->yAxis2->setRange(0, 1200);
    customPlot->yAxis2->setSubTicks(false);
    customPlot->yAxis2->setTickLength(0, 6);

    // 这里之所以用 QCPAxisTickerText，而不用 QCPAxisTickerTime / QCPAxisTickerDateTime 是因为它们会自动计算刻度线的位置，这是我们不想要的
    QSharedPointer<QCPAxisTickerText> timeTicker(new QCPAxisTickerText);
    QSharedPointer<QCPAxisTickerText> indexTicker(new QCPAxisTickerText);
    customPlot->xAxis->setTicker(timeTicker);
    customPlot->xAxis->setSubTicks(false);
    customPlot->xAxis2->setVisible(true);
    customPlot->xAxis2->setSubTicks(false);
    customPlot->xAxis2->setTicker(indexTicker);
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));  // 重要：让xAxis2与xAxis保持联动

    // 添加数据
    QVector<double> datax, datay1, datay2;
    QTime now = QTime::currentTime();
    for (int i = 0; i < 8; ++i) {
        mLabels.insert(0, now.toString("hh:mm:ss"));  // mLabels == QVector<QString>
        datax.insert(0, now.msecsSinceStartOfDay() / 1000.0);  // 时间
        now = now.addMSecs(-2000);

        mIndexLabels.insert(0, QString::number(8 - i));
        datay1.insert(0, QRandomGenerator::global()->bounded(20) + 5);   // 产生随机数，Qt 5.10 以下用qrand() % 20
        datay2.insert(0, QRandomGenerator::global()->bounded(1000));
    }

    mIndex = 8;
    mPositions = datax; // mPositions == QVector<double>

    graph->setData(datax, datay1);
    bar->setData(datax, datay2);

    timeTicker->setTicks(mPositions, mLabels);   // 设置刻度线位置和刻度标签
    indexTicker->setTicks(mPositions, mIndexLabels);

    customPlot->xAxis->rescale();
    customPlot->xAxis->scaleRange(1.1);  // 稍微缩放一下，以显示全部

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(dynamicDataSlot()));
    dataTimer.start(2000);  // 2秒刷新一次
}

void MainWindow::dynamicDataSlot()
{
    QCPBars *bar = static_cast<QCPBars *>(mCustomPlot->plottable(0));

    // 1. 去除之前的数据
    // 移除第一个数据
    bar->data()->remove(mPositions.first());
    mCustomPlot->graph(0)->data()->remove(mPositions.first());

    mPositions.removeFirst();
    mLabels.removeFirst();
    mIndexLabels.removeFirst();

    // 2. 添加新增数据
    // 计算当前时间
    double key = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;
    QString label = QTime::currentTime().toString("hh:mm:ss");

    mCustomPlot->graph(0)->addData(key, QRandomGenerator::global()->bounded(20) + 5);
    bar->addData(key, QRandomGenerator::global()->bounded(1000));

    mPositions.push_back(key);
    mLabels.push_back(label);
    mIndexLabels.push_back(QString::number(++mIndex));

    // 3. 重新设置轴并刷新图表
    QCPAxisTickerText *timeTicker = static_cast<QCPAxisTickerText *>(mCustomPlot->xAxis->ticker().data());
    timeTicker->setTicks(mPositions, mLabels);  // 重新设置x轴

    QCPAxisTickerText *indexTicker = static_cast<QCPAxisTickerText *>(mCustomPlot->xAxis2->ticker().data());
    indexTicker->setTicks(mPositions, mIndexLabels);

    mCustomPlot->xAxis->rescale();
    mCustomPlot->xAxis->scaleRange(1.1);

    mCustomPlot->replot();

}
