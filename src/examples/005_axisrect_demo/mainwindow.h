#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QCustomPlot;

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget* parent = nullptr);

    void setupAdvancedAxesDemo(QCustomPlot *customPlot);

private:

};

#endif // MAINWINDOW_H
