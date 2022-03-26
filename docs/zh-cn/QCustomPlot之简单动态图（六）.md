本章节是仿照echart的动态图：[地址](https://www.echartsjs.com/examples/zh/editor.html?c=dynamic-data)

![最终效果图](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC0yNzdhZDU4ZWQ5ZjFmYzliLmdpZg#pic_center)

#### 一、设置布局

首先新增一行一列用于放置标题和图例，这里将图例放置在一个空的轴矩形当中--[QCustomPlot之层和布局（四）](https://www.jianshu.com/p/4c4c0e2d01e8)


```
// setupDynamicData(QCustomPlot *customPlot) 函数中
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
```

#### 二、设置图表

其次添加一个柱状图和折线图，并且添加它们的数据，注意这里采用`QCPAxisTickerText`而不采用`QCPAxisTickerTime`或者`QCPAxisTickerDateTime`的原因，是因为我们不想要它们计算刻度线的位置，同时要注意让`xAxis2`保持与`xAxis`的联动，这里只需连接信号槽即可

```
// setupDynamicData(QCustomPlot *customPlot) 函数中
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
```

#### 三、连接计时信号槽
```
// setupDynamicData(QCustomPlot *customPlot) 函数中
connect(&dataTimer, SIGNAL(timeout()), this, SLOT(dynamicDataSlot()));
dataTimer.start(2000);  // 2秒刷新一次
```

#### 四、设置数据刷新的槽函数

以下都是在`dynamicDataSlot`槽函数中

- 去除之前的数据

```
QCPBars *bar = static_cast<QCPBars *>(ui->customPlot->plottable(0));

// 移除第一个数据
bar->data()->remove(mPositions.first());
ui->customPlot->graph(0)->data()->remove(mPositions.first());

mPositions.removeFirst();
mLabels.removeFirst();
mIndexLabels.removeFirst();
```

- 添加新增数据

```
// 计算当前时间
double key = QTime::currentTime().msecsSinceStartOfDay() / 1000.0;
QString label = QTime::currentTime().toString("hh:mm:ss");

ui->customPlot->graph(0)->addData(key, QRandomGenerator::global()->bounded(20) + 5);
bar->addData(key, QRandomGenerator::global()->bounded(1000));

mPositions.push_back(key);
mLabels.push_back(label);
mIndexLabels.push_back(QString::number(++mIndex));
```

- 重新设置轴并刷新图表

```
QCPAxisTickerText *timeTicker = static_cast<QCPAxisTickerText *>(ui->customPlot->xAxis->ticker().data());
timeTicker->setTicks(mPositions, mLabels);  // 重新设置x轴

QCPAxisTickerText *indexTicker = static_cast<QCPAxisTickerText *>(ui->customPlot->xAxis2->ticker().data());
indexTicker->setTicks(mPositions, mIndexLabels);

ui->customPlot->xAxis->rescale();
ui->customPlot->xAxis->scaleRange(1.1);

ui->customPlot->replot();
```

