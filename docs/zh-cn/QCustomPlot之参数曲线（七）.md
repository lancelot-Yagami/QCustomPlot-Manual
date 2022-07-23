`QCPCurve`用于绘制参数方程曲线，常见的参数方程有以下几种：

![常见参数方程曲线](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC03MmQ2NmMzNzNmNjBlNTZiLnBuZw?x-oss-process=image/format,png#pic_center)

`QCPCurve`与`QCPGraph`的区别在于它引入了第三个坐标`t`，而`QCPGraph`只有x,y两个坐标，这是因为在参数方程曲线中，可能会有多个点对应同个key坐标，而`t`坐标的引入决定了参数方程x,y坐标的顺序，这样就不会混乱

在数据方面，`t`坐标在QCPCurve表现为排序键`sortKey`，`x`坐标表现为主键`mainKey`，`y`坐标表现为`mainValue`，而在QCPGraph中排序键`sortKey`和主键`mainKey`都是`x`坐标，`y`坐标表现为`mainValue`

#### 参数方程的使用

```
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
```

参数方程来自：[伯努利双纽线(无穷大的符号)](https://blog.csdn.net/stereohomology/article/details/45528463)
```
void MainWindow::dynamicLemniscateSlot()
{
    static double t = -M_PI;           // 定义起始t
    static bool reversed = false;   // 为了重复绘制

    QCPCurve *curve = static_cast<QCPCurve *>(ui->customPlot->plottable(0));

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

    ui->customPlot->replot();

    // 重复绘制
    if (t >= M_PI) {
        t = M_PI;
        reversed = true;
    } else if (reversed && t <= -M_PI) {
        t = -M_PI;
        reversed = false;
    }
}
```


![效果图](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC0wZjQzY2M0ZmQwZGE3ODVkLmdpZg#pic_center)
