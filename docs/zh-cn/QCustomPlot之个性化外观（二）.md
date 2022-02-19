QCustomPlot可以制作出优美的图表，它提供了大量可以自定义风格的接口

####  设置QCustomPlot的背景颜色
```
QLinearGradient plotGradient;
plotGradient.setStart(0, 0);
plotGradient.setFinalStop(0, 350);
plotGradient.setColorAt(0, QColor(80, 80, 80));
plotGradient.setColorAt(1, QColor(50, 50, 50));
customPlot->setBackground(plotGradient);      // 设置背景颜色
```
![QCustomPlot背景颜色](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC02ZGUyYjM0Y2FjYWI1ZmIxLnBuZw?x-oss-process=image/format,png#pic_center)


#### 设置QCPAxisRect轴矩形的背景颜色
```
QLinearGradient axisRectGradient;
axisRectGradient.setStart(0, 0);
axisRectGradient.setFinalStop(0, 350);
axisRectGradient.setColorAt(0, QColor(80, 80, 80));
axisRectGradient.setColorAt(1, QColor(30, 30, 30));
customPlot->axisRect()->setBackground(axisRectGradient);   // 设置QCPAxisRect背景颜色
```

![轴矩形的背景颜色](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC05YTMwZGFhNjM2YjVmOWQyLnBuZw?x-oss-process=image/format,png)


#### 设置QCPAxis轴的风格
```
customPlot->xAxis->setBasePen(QPen(Qt::white, 1));  // 轴线的画笔
customPlot->xAxis->setTickPen(QPen(Qt::white, 1));  // 轴刻度线的画笔
customPlot->xAxis->setSubTickPen(QPen(Qt::white, 1)); // 轴子刻度线的画笔
customPlot->xAxis->setTickLabelColor(Qt::white);  // 轴刻度文字颜色
customPlot->xAxis->setLabel("标签");  // 只有设置了标签，轴标签的颜色才会显示
customPlot->xAxis->setLabelColor(Qt::white);   // 轴标签颜色
customPlot->xAxis->setTickLengthIn(3);       // 轴线内刻度的长度
customPlot->xAxis->setTickLengthOut(5);      // 轴线外刻度的长度
customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);  // 设置轴线结束时的风格为 实角三角形但内部有凹陷的形状， setLowerEnding设置轴线开始时的风格
```

![轴风格](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC0zMDQyN2IwYzAyNjgyZWM3LnBuZw?x-oss-process=image/format,png#pic_center)


#### 设置QCPGrid网格的风格
```
// 每条网格对应一个刻度
customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));     // 网格线(对应刻度)画笔
customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine)); // 子网格线(对应子刻度)画笔
customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
customPlot->xAxis->grid()->setSubGridVisible(true);     // 显示子网格线
customPlot->yAxis->grid()->setSubGridVisible(true);
customPlot->xAxis->grid()->setZeroLinePen(QPen(Qt::red));   // 设置刻度为0时的网格线的画笔
customPlot->yAxis->grid()->setZeroLinePen(QPen(Qt::red));
```
![网格线风格](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC00MWY1YmQ4Y2RhMzAwOGZmLnBuZw?x-oss-process=image/format,png#pic_center)


#### 图表的风格
```
QPen pen;
QStringList lineNames;
lineNames << "lsNone" << "lsLine" << "lsStepLeft" << "lsStepRight" << "lsStepCenter" << "lsImpulse";

for (int i = QCPGraph::lsNone; i <= QCPGraph::lsImpulse; ++i)
{
    customPlot->addGraph();
    pen.setColor(QColor(qSin(i*1+1.2)*80+80, qSin(i*0.3+0)*80+80, qSin(i*0.3+1.5)*80+80));
    customPlot->graph()->setPen(pen);       // 设置图表的画笔
    customPlot->graph()->setName(lineNames.at(i-QCPGraph::lsNone));
    customPlot->graph()->setLineStyle((QCPGraph::LineStyle)i);  // 设置图表线段的风格
    customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));  // 设置图表散点图的样式，散点图的样式有很多种，可以自己试试

    QVector<double> x(15), y(15);
    for (int j=0; j<15; ++j)
    {
        x[j] = j/15.0 * 5*3.14 + 0.01;
        y[j] = 7*qSin(x[j])/x[j] - (i-QCPGraph::lsNone)*5 + (QCPGraph::lsImpulse)*5 + 2;
    }
    customPlot->graph()->setData(x, y);
    customPlot->graph()->rescaleAxes(true);
}
```

![图表风格](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC0wMTZmNjI1NDZmMzM5YTNhLnBuZw?x-oss-process=image/format,png#pic_center)


#### 图表画刷
```
// 第一种：与0刻度线围成区域
customPlot->addGraph();
customPlot->graph(0)->setPen(QPen(Qt::blue));
customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
customPlot->addGraph();
customPlot->graph(1)->setPen(QPen(Qt::red));

// 第二种方式：与其它图围成区域，使用的是图0的画刷
// customPlot->graph(0)->setChannelFillGraph(customPlot->graph(1));   // 将图0与图1围成区域
```
![图表画刷](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC03MDM4ZjUyOWRiNDRkNjVmLnBuZw?x-oss-process=image/format,png#pic_center)
