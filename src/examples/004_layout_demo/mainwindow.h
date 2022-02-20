#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QCustomPlot;
class QCPLayoutGrid;

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget* parent = nullptr);

    void setupDemo(int index);
    void setupQuadraticDemo(QCustomPlot *customPlot);
    void setupLayoutDemo(QCustomPlot* customPlot);

protected:
    void reset();

private:
    QCustomPlot* mCustomPlot;
    Qt::Orientation mOrientation;

    bool mIsFreeLegend = false;
    bool mIsLegendInCustomPlot = false;
};

#endif // MAINWINDOW_H
