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

    void setupStretchItemDemo(QCustomPlot *customPlot);

private:
    QCustomPlot* mCustomPlot;
};

#endif // MAINWINDOW_H
