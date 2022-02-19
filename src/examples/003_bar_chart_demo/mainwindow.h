#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QCustomPlot;

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget* parent = nullptr);

    void setupDemo(int index);
    void setupBarDemo(QCustomPlot* customPlot);

protected:
    void reset();

private:
    QCustomPlot* mCustomPlot;
    Qt::Orientation mOrientation;

    bool mIsStackedBar = false;
    bool mIsGroupBar = false;
    bool mTextVisible = false;
};

#endif // MAINWINDOW_H
