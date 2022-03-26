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

    void setupDynamicData(QCustomPlot *customPlot);

protected slots:
    void dynamicDataSlot();

private:

    int mIndex;
    QVector<double> mPositions;
    QVector<QString> mLabels;
    QVector<QString> mIndexLabels;

    QTimer dataTimer;
    QCustomPlot* mCustomPlot;
};

#endif // MAINWINDOW_H
