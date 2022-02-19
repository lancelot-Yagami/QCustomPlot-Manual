#include "mainwindow.h"

#include <QVBoxLayout>
#include <QComboBox>

#include "qcustomplot.h"
#include "qcptextbars.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mCustomPlot = new QCustomPlot;

    setWindowTitle("QCustomPlot: Bar Chart Demo");

    QWidget* w = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    w->setLayout(layout);
    setCentralWidget(w);

    QStringList items;
    items << "竖向柱状图" << "横向柱状图" << "竖向堆积图" << "横向堆积图" << "竖向分组图" << "横向分组图" << "竖向文字显示" << "横向文字显示";
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
    mIsStackedBar = false;
    mIsGroupBar = false;
    mTextVisible = false;
}

void MainWindow::setupDemo(int index)
{
    reset();

    switch (index) {
    case 0: mOrientation = Qt::Vertical; setupBarDemo(mCustomPlot); break;
    case 1: mOrientation = Qt::Horizontal; setupBarDemo(mCustomPlot); break;
    case 2: mOrientation = Qt::Vertical; mIsStackedBar = true; setupBarDemo(mCustomPlot); break;
    case 3: mOrientation = Qt::Horizontal; mIsStackedBar = true; setupBarDemo(mCustomPlot); break;
    case 4: mOrientation = Qt::Vertical; mIsGroupBar = true; setupBarDemo(mCustomPlot); break;
    case 5: mOrientation = Qt::Horizontal; mIsGroupBar = true; setupBarDemo(mCustomPlot); break;
    case 6: mOrientation = Qt::Vertical; mTextVisible = true;  mIsGroupBar = true; setupBarDemo(mCustomPlot); break;
    case 7: mOrientation = Qt::Horizontal; mTextVisible = true; mIsGroupBar = true; setupBarDemo(mCustomPlot); break;
    }
}

void MainWindow::setupBarDemo(QCustomPlot *customPlot)
{
    QCPAxis *keyAxis = customPlot->xAxis;
    QCPAxis *valueAxis = customPlot->yAxis;

    // 水平方向
    if (mOrientation == Qt::Horizontal) {
        keyAxis = customPlot->yAxis;
        valueAxis = customPlot->xAxis;
    }

    auto *regen = new QCPTextBars(keyAxis, valueAxis);
    auto *nuclear = new QCPTextBars(keyAxis, valueAxis);
    auto *fossil = new QCPTextBars(keyAxis, valueAxis);  // 使用xAxis作为柱状图的key轴，yAxis作为value轴
    regen->setTextVisible(mTextVisible);
    nuclear->setTextVisible(mTextVisible);
    fossil->setTextVisible(mTextVisible);

    regen->setAntialiased(false); // 为了更好的边框效果，关闭抗齿锯
    nuclear->setAntialiased(false); // 为了更好的边框效果，关闭抗齿锯
    fossil->setAntialiased(false); // 为了更好的边框效果，关闭抗齿锯

    regen->setName("Regenerative");
    regen->setPen(QPen(QColor(111, 9, 176).lighter(170)));
    regen->setBrush(QColor(111, 9, 176));
    nuclear->setName("Nuclear");
    nuclear->setPen(QPen(QColor(250, 170, 20).lighter(150)));
    nuclear->setBrush(QColor(250, 170, 20));

    fossil->setName("Fossil fuels");                                         // 设置柱状图的名字，可在图例中显示
    fossil->setPen(QPen(QColor(0, 168, 140).lighter(130)));   // 设置柱状图的边框颜色
    fossil->setBrush(QColor(0, 168, 140));                              // 设置柱状图的画刷颜色

    // 为柱状图设置一个文字类型的key轴，ticks决定了轴的范围，而labels决定了轴的刻度文字的显示
    QVector<double> ticks;
    QVector<QString> labels;
    ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7;
    labels << "USA" << "Japan" << "Germany" << "France" << "UK" << "Italy" << "Canada";
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);

    keyAxis->setTicker(textTicker);        // 设置为文字轴

    keyAxis->setTickLabelRotation(60);     // 轴刻度文字旋转60度
    keyAxis->setSubTicks(false);           // 不显示子刻度
    keyAxis->setTickLength(0, 4);          // 轴内外刻度的长度分别是0,4,也就是轴内的刻度线不显示
    keyAxis->setRange(0, 8);               // 设置范围
    keyAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

    valueAxis->setRange(0, 12.1);
    valueAxis->setPadding(35);             // 轴的内边距，可以到QCustomPlot之开始（一）看图解
    valueAxis->setLabel("Power Consumption in\nKilowatts per Capita (2007)");
    valueAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    QVector<double> fossilData, nuclearData, regenData;
    fossilData  << 0.86*10.5 << 0.83*5.5 << 0.84*5.5 << 0.52*5.8 << 0.89*5.2 << 0.90*4.2 << 0.67*11.2;
    nuclearData << 0.08*10.5 << 0.12*5.5 << 0.12*5.5 << 0.40*5.8 << 0.09*5.2 << 0.00*4.2 << 0.07*11.2;
    regenData   << 0.06*10.5 << 0.05*5.5 << 0.04*5.5 << 0.06*5.8 << 0.02*5.2 << 0.07*4.2 << 0.25*11.2;
    fossil->setData(ticks, fossilData);
    nuclear->setData(ticks, nuclearData);
    regen->setData(ticks, regenData);

    if (mIsStackedBar) {
        regen->setStackingGap(1);  // 设置堆积在其它柱状图上时的间距(像素)
        nuclear->setStackingGap(1);

        nuclear->moveAbove(fossil);
        regen->moveAbove(nuclear); // 将nuclear移到fossil之上
    } else if (mIsGroupBar) {
        QCPBarsGroup *group = new QCPBarsGroup(customPlot);

        QList<QCPBars*> bars;
        bars << fossil << nuclear << regen;

        foreach (QCPBars *bar, bars) {
          // 设置柱状图的宽度类型为以key坐标轴计算宽度的大小，其实默认就是这种方式
          bar->setWidthType(QCPBars::wtPlotCoords);
          bar->setWidth(bar->width() / bars.size()); // 设置柱状图的宽度大小
          group->append(bar);  // 将柱状图加入柱状图分组中
        }

        group->setSpacingType(QCPBarsGroup::stAbsolute);  // 设置组内柱状图的间距，按像素
        group->setSpacing(2);     // 设置较小的间距值，这样看起来更紧凑
    } else {
        regen->setVisible(false);
        nuclear->setVisible(false);
    }

    customPlot->replot();
}


