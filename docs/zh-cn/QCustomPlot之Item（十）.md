在QCustomPlot中，所有的Item都继承自`QCPAbstractItem`，QCustomPlot给我们提供了一些Item

![QCustomPlot中所有的Item](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC1iOTA3NzZhN2VlYTg1MjdjLnBuZw?x-oss-process=image/format,png)

- QCPItemBracket 括号Item
- QCPItemCurve 曲线Item，其实就是贝塞尔曲线
- QCPItemEllipse 椭圆/圆Item
- QCPItemLine 线段Item
- QCPItemPixmap 图片Item
- QCPItemRect 矩形Item
- QCPItemStraightLine 直线Item
- QCPItemText 文字Item
- QCPItemTracer 追踪者Item

`QCPAbstractItem`默认存在于`QCustomPlot`的当前层中，默认的当前层为`main`，我们可以通过`setClipToAxisRect`和`setClipAxisRect`设置Item裁切于某个特定的轴矩形，将`setClipToAxisRect`设置为false则Item裁切于`QCustomPlot`

#### QCPItemAnchor与QCPItemPosition联系与区别
QCPItemPosition继承自QCPItemAnchor，QCPItemAnchor用于给QCPItemPosition定位用的，QCPItemPosition则表示了Item的位置，一个Item可以有多个QCPItemAnchor和QCPItemPosition
我们主要理解QCPItemPosition是如何决定Item的位置的，它有四种位置类型：
- ptAbsolute 按像素的方式
- ptViewportRatio 按QCustomPlot可视区域的比例
- ptAxisRectRatio 按其存在的轴矩形的比例(通过`setAxisRect`函数设置轴矩形)
- ptPlotCoords 按坐标系位置(通过`setAxes`函数轴)

可以通过`setType`、`setTypeX`、`setTypeY`函数设置x、y方向的位置类型，然后通过`setCoords`设置其位置，`setCoords`是跟`setType`设置的类型相关的，Item的位置还跟它锚定的父锚点QCPItemAnchor相关，可以通过`setParentAnchor`、`setParentAnchorX`、`setParentAnchorY`函数设置x、y方向的父锚点，父锚点决定了其起点位置，**当设置父锚点后，type类型会变为ptAbsolute按像素的方式**

我们可以直接通过`setPixelPosition`设置Item的像素位置，而不用管`setType`的类型是什么，通过`pixelPosition`返回Item的像素位置

**以下图片空心圆为QCPItemAnchor，实心圆为QCPItemPosition**
#### QCPItemBracket 

![QCPItemBracket ](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC1kNGZjNWQwNDgyMzNmMGEwLnBuZw?x-oss-process=image/format,png)
可以看到QCPItemBracket通过`left`和`right`决定其位置，而`center`则用于给其它的QCPItemPosition定位用的，QCPItemBracket通过`setLength`设置`left`/`right`到`center`的垂直距离(像素)
![setLength设置的长度](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC1lNDk4NDBmNDZiYzdjZWRjLnBuZw?x-oss-process=image/format,png)

QCPItemBracket有四种类型：
- bsSquare 中括号
- bsRound 小括号
- bsCurly 大括号
- bsCalligraphic 实体大括号

#### QCPItemCurve

![QCPItemCurve](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC04ZGI1NzZjN2U0OGJiNDc1LnBuZw?x-oss-process=image/format,png)
可以看到QCPItemCurve有四个点决定其位置起点`start`和终点`end`以及两个控制点`startDir`、`endDir`，理解QCPItemCurve的位置需要我们对贝塞尔曲线有所理解

#### QCPItemText 

![QCPItemText](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC0zMjZkYTk2MDdlOTFhYTJkLnBuZw?x-oss-process=image/format,png)
可以看到QCPItemText由`position`决定其位置，但是`position`不一定是在文字中心的位置，我们可以通过`setPositionAlignment
`设置`position`对齐在文字的哪一个地方，比如要对齐在`topLeft`的位置：setPositionAlignment(Qt::AlignLeft)

#### QCPItemTracer
tracer是比较常用的一个Item，它主要用来实时跟踪`QCPGraph`的某个数据点，QCPItemTracer也是用一个`position`来决定其位置

![QCPItemTracer](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC0xMzM4NjU3MDRkMzMzYTBmLnBuZw?x-oss-process=image/format,png)

QCPItemTracer有五种风格
- tsNone 不显示tracer
- tsPlus 加号
- tsCrosshair 十字线，横跨整个轴矩形
- tsCircle 圆
- tsSquare 方块

QCPItemTracer通过函数`setGraph`和`setGraphKey`设置要跟踪的图表以及数据点，`setGraph(nullptr)`可以使QCPItemTracer得位置是任意的。QCPItemTracer的位置还跟`setInterpolating`函数有关，当`setInterpolating`为true时，启用线性插值，这时候QCPItemTracer的位置是精确地在我们设置的位置，并且对`value`轴的值进行线性插值；为false的时候QCPItemTracer的位置是最接近于这个位置的`key`值点的位置