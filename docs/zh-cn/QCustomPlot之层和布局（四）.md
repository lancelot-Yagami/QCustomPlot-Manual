#### 层和层元素

在QCustomPlot中，一切可显示的对象都是继承自层元素`QCPLayerable`，层`QCPLayer`则管理着层元素，QCustomPlot利用层决定了不同元素的绘制顺序
层有两种刷新模式：
- lmLogical 只用于渲染的顺序，并且与`相邻`的lmLogical层共享绘图缓存（注意这里的相邻，如果前一个层是`lmBuffered`模式，则会新建一个绘图缓存用于绘图）
- lmBuffered 层拥有自己的绘图缓存，并且可以单独的刷新（调用`QCPLayer::replot`函数）

默认只有`overlay`层启用了`lmBuffered`单独绘制机制，其它层都共享一个绘图缓存，因为`overlay`层主要是用于存放悬浮与图表上的item项，需要频繁的刷新，启用了`lmBuffered`的层会多占用内存

#### 布局和布局元素
在QCustomPlot中，布局方式只有两种：

* QCPLayoutGrid 网格布局
* QCPLayoutInset 内嵌式布局， 这种布局的作用是把子元素与布局的边界对齐或者将其放置在布局内的任意位置(图例QCPLegend就被放置在此布局内)，严格来说这个并不算布局

我们可以看下继承关系图，可以看到这两种布局都继承自`QCPLayout`，而`QCPLayout`继承自`QCPLayoutElement`布局元素，所以一个布局可以包含另一个布局，布局内的元素都要继承自`QCPLayoutElement`

![继承关系图](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC1iZmQxZDY3NDNkNTZlODljLnBuZw?x-oss-process=image/format,png#pic_center)


在QCustomPlot里面维护着一个网格布局，默认只有一个QCPAxisRect元素，而QCPAxisRect里则维护这一个内嵌式的布局，用于放置图例QCPLegend

```
mPlotLayout = new QCPLayoutGrid;             
mPlotLayout->initializeParentPlot(this);
mPlotLayout->setParent(this);
mPlotLayout->setLayer(QLatin1String("main"));   // 设置网格布局的层为main层

QCPAxisRect *defaultAxisRect = new QCPAxisRect(this, true);
mPlotLayout->addElement(0, 0, defaultAxisRect);
xAxis = defaultAxisRect->axis(QCPAxis::atBottom);
yAxis = defaultAxisRect->axis(QCPAxis::atLeft);
xAxis2 = defaultAxisRect->axis(QCPAxis::atTop);
yAxis2 = defaultAxisRect->axis(QCPAxis::atRight);
legend = new QCPLegend;
legend->setVisible(false);                    // 默认不显示图例
defaultAxisRect->insetLayout()->addElement(legend, Qt::AlignRight | Qt::AlignTop);   // 设置图例对齐于QCPAxisRect的右上角
defaultAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));                // 设置布局的外边距

defaultAxisRect->setLayer(QLatin1String("background"));                              // QCPAxisRect的层设置为background层
```

于是我们可以设置图例的位置
```
customPlot->legend->setVisible(true);           // 因为图例默认为隐藏的，所以让其显示
customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignHCenter);   // 对齐于QCPAxisRect中间的顶部

// 以任意位置的方式放置图例，解开以下的注释查看效果
// customPlot->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
// customPlot->axisRect()->insetLayout()->setInsetRect(0, QRectF(0.6, 0.0, 0.2, 0.2));         // 这里按比例给出图例的位置，在QCPAxisRect的(60%,40%)的位置，宽高分别为QCPAxisRect的(20%,20%)宽高的大小
```

![对齐边界的方式](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC05Y2Y1M2ZlOWI4MTJkNDg1LnBuZw?x-oss-process=image/format,png#pic_center)
![自由位置的方式](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC0zMmQ2OGFjODNlNTNlMzVhLnBuZw?x-oss-process=image/format,png#pic_center)


我们甚至可以把它放置在QCustomPlot中，而不是在QCPAxisRect中，但是这种做法不太可取，just for fun :)

```
customPlot->plotLayout()->setMargins(QMargins(0, 0, 0, 30));           // 首先为底部预留出足够的空间
customPlot->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);   // 设置以任意位置放置图例，以对齐边界的方式不行
customPlot->axisRect()->insetLayout()->setInsetRect(0, QRectF(0.0, 1.2, 1.0, 0.1));    // 将y方向的位置设置为超过1以便它的位置超过QCPAxisRect的位置
```

![图例放置在QCustomPlot中](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC03ZWUwOTAxNjg3MmU3OTVlLnBuZw?x-oss-process=image/format,png#pic_center)

其实我们还可以将图例放置在QCustomPlot的网格布局中，这样也可以将图例放置在QCustomPlot中，但这种方法写起来比较麻烦，这里就不讲解了


#### 为图表添加标题

有了布局，我们可以为所欲为，添加标题很简单，仅需`QCPTextElement`类即可，它也是一个布局元素
```
customPlot->plotLayout()->insertRow(0);     // 插入一行
customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(customPlot, "Regenerative Energies", QFont("sans", 12, QFont::Bold)));  // 在第一行第一列添加标题

```

![为图表添加标题](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC05MDNmYWYwODU1NjlhOTQ5LnBuZw?x-oss-process=image/format,png#pic_center)


#### 为图例添加标题
`QCPLegend`继承自`QCPLayoutGrid`，所以我们也很容易为图例添加一个标题

```
QCPTextElement *title = new QCPTextElement(ui->customPlot, "这是图例标题", QFont("sans", 10, QFont::Bold));
title->setMargins(QMargins(0, 6, 0, 10));      // 为了效果更好，添加一些边距
title->setLayer("legend");                     // 一定要把标题的层设置为legend层

customPlot->legend->insertRow(0);              // 插入一行
customPlot->legend->addElement(0, 0, title);   // 在第一行第一列添加标题
```

![为图例添加标题](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC1mZmU0YjMyNjdjMWQ3MTNiLnBuZw?x-oss-process=image/format,png#pic_center)

#### 横向图例
只需将图例布局的填充顺序改为以列的方式（图例默认是以行的方式，所以是竖向的）
```
customPlot->legend->setFillOrder(QCPLayoutGrid::foColumnsFirst);
```