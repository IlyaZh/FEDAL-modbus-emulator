#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "modbusrtu.h"

#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updatePorts();

    void on_comRefreshButton_clicked();

    void on_comConnectButton_clicked(bool checked);

private:
    Ui::MainWindow *ui;
    ModBusRtu* modbus;
};

#endif // MAINWINDOW_H
