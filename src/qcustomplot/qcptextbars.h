#ifndef QCPTEXTBARS_H
#define QCPTEXTBARS_H

#include "qcustomplot.h"

class QCP_LIB_DECL QCPTextBars : public QCPBars
{
    Q_OBJECT
public:
    explicit QCPTextBars(QCPAxis *keyAxis, QCPAxis *valueAxis);

    Qt::Alignment textAligment() const { return mTextAlignment; }
    bool textVisible() const { return mTextVisible; }
    double spacing() const { return mSpacing; }
    QFont font() const { return mFont; }

    void setTextAlignment(Qt::Alignment alignment);
    void setTextVisible(bool visible);
    void setSpacing(double spacing);
    void setFont(const QFont &font);

protected:
    void draw(QCPPainter *painter) Q_DECL_OVERRIDE;

    virtual void drawText(QCPPainter* painter, double key, double value, const QRectF& barRect);

protected:
    Qt::Alignment mTextAlignment;    // 文字对齐方式
    bool mTextVisible;                         // 文字是否可见
    double mSpacing;                          // 文字与柱状图的间距，这里按像素大小
    QFont mFont;                                 // 文字使用的字体
};

#endif // QCPTEXTBARS_H
