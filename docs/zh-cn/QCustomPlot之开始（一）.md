## 一、前言
QCustomPlot是Qt的一个小型第三方图表库，支持静态/动态曲线、柱状图、蜡烛图、频谱图等。使用方便，

使用方法：
* 直接使用：包含qcustomplot.h和qcustomplot.cpp文件即可
* 生成动态库使用：在生成动态库时定义宏：`QCUSTOMPLOT_COMPILE_LIBRARY`，在使用动态库时定义宏：`QCUSTOMPLOT_USE_LIBRARY`

下载地址：https://www.qcustomplot.com/index.php/download

## 二、简单介绍QCustomPlot的几个重要类

* QCustomPlot 图表类：用于图表的显示和交互
* QCPLayer 图层：管理图层元素（QCPLayerable），所有可显示的对象都是继承自图层元素
* QCPAbstractPlottable 绘图元素：包含 折线图(QCPGraph)、曲线图(QCPCurve)、柱状图(QCPBars)、QCPStatiBox(盒子图)、QCPColorMap(色谱图)、QCPFinancial(金融图)
* QCPAxisRect 坐标轴矩形：一个坐标轴矩形默认包含上下左右四个坐标轴，但是可以添加多个坐标轴
 
## 三、简单介绍QCustomPlot类

QCustomPlot类管理着所有的图层，它默认自带了六个图层，分别是：

1.  背景层`background`
2.  网格层`grid`
3.  绘图层`main`
4.  坐标轴层`axes`
5.  图例层`legend`
6.  overlay层`overlay`

依据层的顺序的不同，绘制的顺序也不同，越在底下的层越早绘制，当前层默认为绘图层`main`

而我们的绘图区域则在QCPAxisRect中，QCustomPlot类默认包含一个QCPAxisRect，我们可以在下图中可以看到一个QCPAxisRect一般来说会有上轴`xAxis2`、下轴`xAxis`、左轴`yAxis`和右轴`yAxis2`四个轴

![QCPAxisRect和QCPAxis的关系图](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC0wZjdhYzdiYWQzMzI2YTEwLnBuZw?x-oss-process=image/format,png#pic_center)



## 四、简单使用

我们以官方的一个简单示例来结束这篇
```
void MainWindow::setupQuadraticDemo(QCustomPlot *customPlot)
{
    QVector<double> x(101), y(101);
    for (int i = 0; i < 101; ++i) {
        x[i] = i / 50.0 - 1; // -1 到 1
        y[i] = x[i] * x[i];
    }

    customPlot->addGraph();                       // 添加一个曲线图QGraph
    customPlot->graph(0)->setData(x, y);          // 为曲线图添加数据
    customPlot->graph(0)->setName("第一个示例");   // 设置曲线图的名字
    customPlot->xAxis->setLabel("x");             // 设置x轴的标签
    customPlot->yAxis->setLabel("y");
    customPlot->xAxis->setRange(-1, 1);           // 设置x轴的范围为(-1,1)
    customPlot->yAxis->setRange(0, 1);
    customPlot->legend->setVisible(true);         // 显示图例
}
```

效果图

![demo1.png](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC01YWRjZjMzYWU1NmMxYmY5LnBuZw?x-oss-process=image/format,png#pic_center)
