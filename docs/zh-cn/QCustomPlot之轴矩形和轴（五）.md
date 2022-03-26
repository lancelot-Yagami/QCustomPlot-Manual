#### 轴矩形QCPAxisRect

轴矩形的作用主要有以下几种：

- 背景绘制，所以轴矩形默认存在于`background`层
- 管理多个轴，并不仅限于上下左右四个轴
- 图表的拖拽和缩放

先来看下轴矩形的构造函数，可以看到，轴矩形管理着上下左右四个方向的轴，同时当`setupDefaultAxes`为true时(默认为true)时，将会添加上下左右四个轴，并且设置下轴(x轴)和左轴(y轴)为拖拽和缩放时影响的轴

```
QCPAxisRect::QCPAxisRect(QCustomPlot *parentPlot, bool setupDefaultAxes)
    : QCPLayoutElement(parentPlot),
      mBackgroundBrush(Qt::NoBrush),
      mBackgroundScaled(true),
      mBackgroundScaledMode(Qt::KeepAspectRatioByExpanding),
      mInsetLayout(new QCPLayoutInset),
      mRangeDrag(Qt::Horizontal | Qt::Vertical),           // 水平和竖直方向都可以拖拽
      mRangeZoom(Qt::Horizontal | Qt::Vertical),           // 水平和竖直方向都可以缩放
      mRangeZoomFactorHorz(0.85),                          // 水平缩放比例
      mRangeZoomFactorVert(0.85),                          // 竖直缩放比例
      mDragging(false)
{
    // 其余无关代码

    mAxes.insert(QCPAxis::atLeft, QList<QCPAxis*>());
    mAxes.insert(QCPAxis::atRight, QList<QCPAxis*>());
    mAxes.insert(QCPAxis::atTop, QList<QCPAxis*>());
    mAxes.insert(QCPAxis::atBottom, QList<QCPAxis*>());

    if (setupDefaultAxes) {
        QCPAxis *xAxis = addAxis(QCPAxis::atBottom);
        QCPAxis *yAxis = addAxis(QCPAxis::atLeft);
        QCPAxis *xAxis2 = addAxis(QCPAxis::atTop);
        QCPAxis *yAxis2 = addAxis(QCPAxis::atRight);
        setRangeDragAxes(xAxis, yAxis);                        // 设置拖拽时影响的轴
        setRangeZoomAxes(xAxis, yAxis);                        // 设置缩放时影响的轴
        
        // 其余无关代码
    }
}
```


于是我们可以知道，轴矩形是怎么影响图表的拖拽和缩放的：
1. mRangeDrag、mRangeZoom作用的方向，由setRangeDrag和setRangeZoom函数设置
2. setRangeDragAxes和setRangeZoomAxes函数设置的拖拽和缩放时影响的轴(可以是多个轴)


同时轴矩形还提供了函数zoom函数，可以很方便的实现矩形框选缩放的功能

#### 轴QCPAxis

轴的作用主要是：决定图表的范围

轴主要由四部分组成：
- 轴线及刻度线
- 轴刻度文字
- 轴标签
- 网格线

QCPAxis可以为多个图表所共有，同时它提供了非常丰富的接口用于设置上述组成部分的风格，[QCustomPlot之个性化外观（二）](https://www.jianshu.com/p/bdd8b363e6b7)的内容

QCustomPlot还为我们提供了几种默认的轴（ticker, 其实是用于产生刻度和刻度文字的）：
- QCPAxisTickerFixed // 固定步进的轴
- QCPAxisTickerLog // log轴
- QCPAxisTickerPi  // Pi轴
- QCPAxisTickerText  // 文字轴
- QCPAxisTickerDateTime // 日期轴
- QCPAxisTickerTime  // 时间轴

使用**setTicker**函数设置相应的ticker就可以了

![多种轴类型](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC02NjQxMTc0ZjRkYjI1ZjNkLnBuZw?x-oss-process=image/format,png#pic_center)

**setNumberFormat**函数的介绍：用于轴标签文字的显示格式，可以由以下三个字符组成
- 第一个字符可以为：'e'/'E'、'g'/'G'、'f'，这里使用了Qt的显示风格
- 第二个字符必须为：'b'，当且仅当第一个字符为'e'(科学计数法)或者'g'(精度为最大有效位数)时才有效，b即beautiful，可以让轴刻度文字看起来更好看
- 第三个字符可以为：'c'(cross)、'd'(dot)，即表现为乘号或者点


#### 总结
- 轴矩形可以拥有多个轴
- 多个图表可以共用一个轴
- QCustomPlot可以存在多个轴矩形，因为轴矩形继承自`QCPLayoutElement`


```
void MainWindow::setupAdvancedAxesDemo(QCustomPlot *customPlot)
{
    demoName = "Advanced Axes Demo";

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
```

![多个轴与多个轴矩形的效果图](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC1jMWJjNDBhM2MyNmM4NTE0LnBuZw?x-oss-process=image/format,png#pic_center)
