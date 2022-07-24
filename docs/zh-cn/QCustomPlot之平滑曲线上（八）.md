在QCustomPlot中，并没有为我们提供平滑曲线，所以需要我们改造它

#### 一、生成平滑曲线
**2020-6-6日更新修复当数据中有NaN的数据时平滑曲线的显示问题**
来源：[公孙二狗](https://qtdebug.com/qtbook-paint-smooth-curve/) ，在这里感谢狗哥
生成平滑曲线的方法我也不知道，反正拿来用就是了
```
class SmoothCurveGenerator
{
protected:
    static QPainterPath generateSmoothCurveImp(const QVector<QPointF> &points) {
        QPainterPath path;
        int len = points.size();

        if (len < 2) {
            return path;
        }

        QVector<QPointF> firstControlPoints;
        QVector<QPointF> secondControlPoints;
        calculateControlPoints(points, &firstControlPoints, &secondControlPoints);

        path.moveTo(points[0].x(), points[0].y());

        // Using bezier curve to generate a smooth curve.
        for (int i = 0; i < len - 1; ++i) {
            path.cubicTo(firstControlPoints[i], secondControlPoints[i], points[i+1]);
        }

        return path;
    }
public:
    static QPainterPath generateSmoothCurve(const QVector<QPointF> &points) {
        QPainterPath result;

        int segmentStart = 0;
        int i = 0;
        int pointSize = points.size();
        while (i < pointSize) {
            if (qIsNaN(points.at(i).y()) || qIsNaN(points.at(i).x()) || qIsInf(points.at(i).y())) {
                QVector<QPointF> lineData(QVector<QPointF>(points.constBegin() + segmentStart, points.constBegin() + i - segmentStart));
                result.addPath(generateSmoothCurveImp(lineData));
                segmentStart = i + 1;
            }
            ++i;
        }
        QVector<QPointF> lineData(QVector<QPointF>(points.constBegin() + segmentStart, points.constEnd()));
        result.addPath(generateSmoothCurveImp(lineData));
        return result;
    }

    static QPainterPath generateSmoothCurve(const QPainterPath &basePath, const QVector<QPointF> &points) {
        if (points.isEmpty()) return basePath;

        QPainterPath path = basePath;
        int len = points.size();
        if (len == 1) {
            path.lineTo(points.at(0));
            return path;
        }

        QVector<QPointF> firstControlPoints;
        QVector<QPointF> secondControlPoints;
        calculateControlPoints(points, &firstControlPoints, &secondControlPoints);

        path.lineTo(points.at(0));
        for (int i = 0; i < len - 1; ++i)
            path.cubicTo(firstControlPoints[i], secondControlPoints[i], points[i+1]);

        return path;
    }

    static void calculateFirstControlPoints(double *&result, const double *rhs, int n) {
        result = new double[n];
        double *tmp = new double[n];
        double b = 2.0;
        result[0] = rhs[0] / b;

        // Decomposition and forward substitution.
        for (int i = 1; i < n; i++) {
            tmp[i] = 1 / b;
            b = (i < n - 1 ? 4.0 : 3.5) - tmp[i];
            result[i] = (rhs[i] - result[i - 1]) / b;
        }

        for (int i = 1; i < n; i++) {
            result[n - i - 1] -= tmp[n - i] * result[n - i]; // Backsubstitution.
        }

        delete[] tmp;
    }

    static void calculateControlPoints(const QVector<QPointF> &knots,
                                                       QVector<QPointF> *firstControlPoints,
                                                       QVector<QPointF> *secondControlPoints) {
        int n = knots.size() - 1;

        firstControlPoints->reserve(n);
        secondControlPoints->reserve(n);

        for (int i = 0; i < n; ++i) {
            firstControlPoints->append(QPointF());
            secondControlPoints->append(QPointF());
        }

        if (n == 1) {
            // Special case: Bezier curve should be a straight line.
            // P1 = (2P0 + P3) / 3
            (*firstControlPoints)[0].rx() = (2 * knots[0].x() + knots[1].x()) / 3;
            (*firstControlPoints)[0].ry() = (2 * knots[0].y() + knots[1].y()) / 3;

            // P2 = 2P1 – P0
            (*secondControlPoints)[0].rx() = 2 * (*firstControlPoints)[0].x() - knots[0].x();
            (*secondControlPoints)[0].ry() = 2 * (*firstControlPoints)[0].y() - knots[0].y();

            return;
        }

        // Calculate first Bezier control points
        double *xs = nullptr;
        double *ys = nullptr;
        double *rhsx = new double[n]; // Right hand side vector
        double *rhsy = new double[n]; // Right hand side vector

        // Set right hand side values
        for (int i = 1; i < n - 1; ++i) {
            rhsx[i] = 4 * knots[i].x() + 2 * knots[i + 1].x();
            rhsy[i] = 4 * knots[i].y() + 2 * knots[i + 1].y();
        }
        rhsx[0] = knots[0].x() + 2 * knots[1].x();
        rhsx[n - 1] = (8 * knots[n - 1].x() + knots[n].x()) / 2.0;
        rhsy[0] = knots[0].y() + 2 * knots[1].y();
        rhsy[n - 1] = (8 * knots[n - 1].y() + knots[n].y()) / 2.0;

        // Calculate first control points coordinates
        calculateFirstControlPoints(xs, rhsx, n);
        calculateFirstControlPoints(ys, rhsy, n);

        // Fill output control points.
        for (int i = 0; i < n; ++i) {
            (*firstControlPoints)[i].rx() = xs[i];
            (*firstControlPoints)[i].ry() = ys[i];

            if (i < n - 1) {
                (*secondControlPoints)[i].rx() = 2 * knots[i + 1].x() - xs[i + 1];
                (*secondControlPoints)[i].ry() = 2 * knots[i + 1].y() - ys[i + 1];
            } else {
                (*secondControlPoints)[i].rx() = (knots[n].x() + xs[n - 1]) / 2;
                (*secondControlPoints)[i].ry() = (knots[n].y() + ys[n - 1]) / 2;
            }
        }

        delete xs;
        delete ys;
        delete[] rhsx;
        delete[] rhsy;
    }
};
```

#### 二、drawLinePlot函数的修改

`mSmooth`为我们添加的一个`bool`型的类成员变量，并且我们限制了`QCPGraph`的线风格`mLineStyle`为`lsLine`的时候才会真正的绘制平滑曲线
```
void QCPGraph::drawLinePlot(QCPPainter *painter, const QVector<QPointF> &lines) const
{
    if (painter->pen().style() != Qt::NoPen && painter->pen().color().alpha() != 0) {
        applyDefaultAntialiasingHint(painter);
        if (mSmooth && mLineStyle == lsLine) painter->drawPath(SmoothCurveGenerator::generateSmoothCurve(lines));
        else drawPolyline(painter, lines);
    }
}
```

这时候已经可以实现平滑曲线了，来个简单的例子看下
```
void MainWindow::setupSmoothCurveDemo(QCustomPlot *customPlot)
{
    QVector<double> xdata = { 1, 2, 3, 4, 5, 6, 7 };
    QVector<double> ydata = { 820, 932, 901, 934, 1290, 1330, 1320 };

    QCPSmoothCurve *graph = new QCPSmoothCurve(customPlot->xAxis, customPlot->yAxis);
    graph->setPen(QPen(Qt::red, 2));
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(Qt::red), QColor(Qt::white), 6));
    graph->setData(xdata, ydata);
    graph->setSmooth(true);   // 开启平滑曲线

    customPlot->xAxis->setRange(0, 8);
    customPlot->yAxis->setRange(0, 1500);
}
```
![平滑曲线与折线对比图](https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8xOTA3NTIxMC00MDUwZTJjNjljZjRmOWY1LnBuZw?x-oss-process=image/format,png)
