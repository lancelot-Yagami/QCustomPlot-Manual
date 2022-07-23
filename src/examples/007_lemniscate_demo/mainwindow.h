#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

class QCustomPlot;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);

    void setupLemniscateDemo(QCustomPlot *customPlot);

protected slots:
    void dynamicLemniscateSlot();

private:
    QTimer dataTimer;
    QCustomPlot* mCustomPlot;
};

#endif // MAINWINDOW_H
