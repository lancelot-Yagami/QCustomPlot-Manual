在QCustomPlot中，柱状图需要使用QCPBars类， QCPBars的使用很简单

#### 竖向柱状图
创建一个柱状图，并且设置它的风格
```
QCPAxis *keyAxis = customPlot->xAxis;
QCPAxis *valueAxis = customPlot->yAxis;
QCPBars *fossil = new QCPBars(keyAxis, valueAxis);  // 使用xAxis作为柱状图的key轴，yAxis作为value轴

fossil->setAntialiased(false); // 为了更好的边框效果，关闭抗齿锯
fossil->setName("Fossil fuels"); // 设置柱状图的名字，可在图例中显示
fossil->setPen(QPen(QColor(0, 168, 140).lighter(130))); // 设置柱状图的边框颜色
fossil->setBrush(QColor(0, 168, 140));  // 设置柱状图的画刷颜色
```

之后，我们为柱状图的key轴设置一个文字类型的轴
```
// 为柱状图设置一个文字类型的key轴，ticks决定了轴的范围，而labels决定了轴的刻度文字的显示
QVector<double> ticks;
QVector<QString> labels;
ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7;
labels << "USA" << "Japan" << "Germany" << "France" << "UK" << "Italy" << "Canada";
QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
textTicker->addTicks(ticks, labels);

keyAxis->setTicker(textTicker);        // 设置为文字轴
```

设置柱状图轴范围，并且为其添加数据数据
```
keyAxis->setTickLabelRotation(60);     // 轴刻度文字旋转60度
keyAxis->setSubTicks(false);           // 不显示子刻度
keyAxis->setTickLength(0, 4);          // 轴内外刻度的长度分别是0,4,也就是轴内的刻度线不显示
keyAxis->setRange(0, 8);               // 设置范围
keyAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

valueAxis->setRange(0, 12.1);
valueAxis->setPadding(35);             // 轴的内边距，可以到QCustomPlot之开始（一）看图解 
valueAxis->setLabel("Power Consumption in\nKilowatts per Capita (2007)");
valueAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
QVector<double> fossilData;
fossilData  << 0.86*10.5 << 0.83*5.5 << 0.84*5.5 << 0.52*5.8 << 0.89*5.2 << 0.90*4.2 << 0.67*11.2;
fossil->setData(ticks, fossilData);
```
![竖向柱状图](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC01ZmQwNjFlYmFmZTlmODc0LnBuZw?x-oss-process=image/format,png#pic_center)

#### 横向柱状图

横向柱状图也很简单，只要设置key轴为y轴，value轴为x轴即可
```
QCPAxis *keyAxis = customPlot->yAxis;
QCPAxis *valueAxis = customPlot->xAxis;
```

![横向柱状图](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC02MGE1Y2UwZWFmNzRiNDk0LnBuZw?x-oss-process=image/format,png#pic_center)

#### 柱状堆积图
为了展示堆积图，我们再添加了两个柱状图
```
QCPBars *regen = new QCPBars(keyAxis, valueAxis);
QCPBars *nuclear = new QCPBars(keyAxis, valueAxis);
QCPBars *fossil = new QCPBars(keyAxis, valueAxis);  // 使用xAxis作为柱状图的key轴，yAxis作为value轴
```

设置它们的数据
```
QVector<double> fossilData, nuclearData, regenData;
fossilData  << 0.86*10.5 << 0.83*5.5 << 0.84*5.5 << 0.52*5.8 << 0.89*5.2 << 0.90*4.2 << 0.67*11.2;
nuclearData << 0.08*10.5 << 0.12*5.5 << 0.12*5.5 << 0.40*5.8 << 0.09*5.2 << 0.00*4.2 << 0.07*11.2;
regenData   << 0.06*10.5 << 0.05*5.5 << 0.04*5.5 << 0.06*5.8 << 0.02*5.2 << 0.07*4.2 << 0.25*11.2;
fossil->setData(ticks, fossilData);
nuclear->setData(ticks, nuclearData);
regen->setData(ticks, regenData);
```

接下来设置堆积方式
```
regen->setStackingGap(1);    // 设置堆积在其它柱状图上时的间距(像素)
nuclear->setStackingGap(1);

nuclear->moveAbove(fossil);  // 将nuclear移到fossil之上
regen->moveAbove(nuclear);
```

![柱状堆积图](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC00YmY2ZTI1NzJiZjk4YjNiLnBuZw?x-oss-process=image/format,png#pic_center)

#### 柱状分组图
柱状分组图需要引入一个新的类：`QCPBarsGroup`，首先注释掉上面的`堆积方式`的代码

```
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
```

![柱状分组图](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC1jNjY2YTM3NWIyZTYwOGY3LnBuZw?x-oss-process=image/format,png#pic_center)

#### 柱状图宽度类型和柱状分组图间距类型

柱状图有三种宽度类型，分别是：

* QCPBars::wtAbsolute  宽度是绝对像素大小，即setWidth设置为多少就是多少  
* QCPBars::wtAxisRectRatio 宽度大小是以QCPAxisRect的大小决定的，当key轴为水平的时候，宽度大小为setWidth设置的比例值乘以QCPAxisRect的宽度;key轴为垂直的时候，是乘以QCPAxisRect的高度
* QCPBars::wtPlotCoords 宽度大小是以key坐标轴刻度位置以及setWidth设置的值确定，宽度的计算方式为当前key±width

柱状分组图的间距类型和柱状图的宽度类型是类似的

#### 在柱状图上显示值
有时候我们需要在柱状图上显示具体的值，这时我们仅需继承`QCPBars`

```
// 头文件CustomBars.h
class CustomBars : public QCPBars
{
public:
    explicit CustomBars(QCPAxis *keyAxis, QCPAxis *valueAxis);

    Qt::Alignment textAligment() const { return mTextAlignment; }
    double spacing() const { return mSpacing; }
    QFont font() const { return mFont; }

    void setTextAlignment(Qt::Alignment alignment);
    void setSpacing(double spacing);
    void setFont(const QFont &font);

protected:
    Qt::Alignment mTextAlignment;   // 文字对齐方式
    double mSpacing;                // 文字与柱状图的间距，这里按像素大小
    QFont mFont;                    // 文字使用的字体

    virtual void draw(QCPPainter *painter) Q_DECL_OVERRIDE;
};
```

```
// 源文件CustomBars.cpp
CustomBars::CustomBars(QCPAxis *keyAxis, QCPAxis *valueAxis)
    : QCPBars (keyAxis, valueAxis),
      mTextAlignment(Qt::AlignCenter),
      mSpacing(5),
      mFont(QFont(QLatin1String("sans serif"), 12))
{

}

void CustomBars::setTextAlignment(Qt::Alignment alignment)
{
    mTextAlignment = alignment;
}

void CustomBars::setSpacing(double spacing)
{
    mSpacing = spacing;
}

void CustomBars::setFont(const QFont &font)
{
    mFont = font;
}
```

**draw函数的绘制** 我们直接拷贝QCPBars的draw函数源码过来，在其上面修改即可
```
// 源文件CustomBars.cpp
void CustomBars::draw(QCPPainter *painter)
{

    // 以上是拷贝的源码部分
    painter->drawPolygon(barRect);

    // 我们仅需在 painter->drawPolygon(barRect); 这行下增加以下的内容即可
    
    // 计算文字的位置
    painter->setFont(mFont);                     // 设置字体
    QString text = QString text = QString::number(it->value, 'g', 2);   // 取得当前value轴的值，保留两位精度

    QRectF textRect = painter->fontMetrics().boundingRect(0, 0, 0, 0, Qt::TextDontClip | mTextAlignment, text);  // 计算文字所占用的大小

    if (mKeyAxis.data()->orientation() == Qt::Horizontal) {    // 当key轴为水平轴的时候
        if (mKeyAxis.data()->axisType() == QCPAxis::atTop)     // 上轴，移动文字到柱状图下面
            textRect.moveTopLeft(barRect.bottomLeft() + QPointF(0, mSpacing));
        else                                                   // 下轴，移动文字到柱状图上面
            textRect.moveBottomLeft(barRect.topLeft() - QPointF(0, mSpacing));
        textRect.setWidth(barRect.width());
        painter->drawText(textRect, Qt::TextDontClip | mTextAlignment, text);
    } else {                                                  // 当key轴为竖直轴的时候
        if (mKeyAxis.data()->axisType() == QCPAxis::atLeft)   // 左轴，移动文字到柱状图右边
            textRect.moveTopLeft(barRect.topRight() + QPointF(mSpacing, 0));
        else                                                  // 右轴，移动文字到柱状图左边
            textRect.moveTopRight(barRect.topLeft() - QPointF(mSpacing, 0));
        textRect.setHeight(barRect.height());
        painter->drawText(textRect, Qt::TextDontClip | mTextAlignment, text);
    }
    
    // 以下是拷贝的源码部分
}
```

使用很简单，仅需将`QCPBars`替换成`CustomBars`
```
CustomBars *regen = new CustomBars(keyAxis, valueAxis);
CustomBars *nuclear = new CustomBars(keyAxis, valueAxis);
CustomBars *fossil = new CustomBars(keyAxis, valueAxis);  // 使用xAxis作为柱状图的key轴，yAxis作为value轴
```

![在柱状图上显示数值](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC02M2NhNTYxZDk4ZmZhZTIyLnBuZw?x-oss-process=image/format,png#pic_center)

#### 最后补充一点
我们可以通过`setBaseValue`函数设置柱状图的起点位置，这个函数仅对最底下的柱状图有效果（即其没有堆积在别的柱状图之上）
```
foreach (QCPBars *bar, bars) {
  // 设置柱状图的宽度类型为以key坐标轴计算宽度的大小，其实默认就是这种方式
  bar->setBaseValue(2.5);     // 设置柱状图的起点为value轴值为2.5的位置
}
```

![设置柱状图起点位置](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC03YjgzYjcwNGRmNmQ4MzYxLnBuZw?x-oss-process=image/format,png#pic_center)
